#include "BlockDB.h"

#include <mw/core/common/Logger.h>

static const DBTable HEADER_TABLE = { "H" };
static const DBTable BLOCK_TABLE = { "B" };

Locked<IBlockDB> BlockDBFactory::Open(const Context::CPtr& pContext, const FilePath& chainPath)
{
    auto pDatabase = Database::Open(pContext, chainPath.GetChild("blocks"));
    auto pChainStore = ChainStore::Load(chainPath.GetChild("chain"));

    return Locked<IBlockDB>(std::make_shared<BlockDB>(pDatabase, pChainStore));
}

IHeader::CPtr BlockDB::GetHeaderByHash(const Hash& hash) const noexcept
{
    LOG_TRACE_F("Loading header {}", hash);

    auto pEntry = m_pDatabase->Get<IHeader>(HEADER_TABLE, hash.ToHex());
    if (pEntry != nullptr)
    {
        LOG_DEBUG_F("Header found for hash {}", hash);
        return pEntry->item;
    }
    else
    {
        LOG_DEBUG_F("No header found for hash {}", hash);
        return nullptr;
    }
}

std::vector<IHeader::CPtr> BlockDB::GetHeadersByHash(const std::vector<Hash>& hashes) const noexcept
{
    LOG_TRACE_F("Loading {} headers", hashes.size());

    std::vector<IHeader::CPtr> headers;
    headers.reserve(hashes.size());

    for (const Hash& hash : hashes)
    {
        auto pEntry = m_pDatabase->Get<IHeader>(HEADER_TABLE, hash.ToHex());
        if (pEntry != nullptr)
        {
            LOG_TRACE_F("Header found for hash {}", hash);
            headers.emplace_back(pEntry->item);
        }
        else
        {
            LOG_DEBUG_F("No header found for hash {}", hash);
        }
    }

    LOG_DEBUG_F("Found {}/{} headers", headers.size(), hashes.size());
    return headers;
}

void BlockDB::AddHeader(const IHeader::CPtr& pHeader)
{
    LOG_TRACE_F("Adding header {}", pHeader);

    std::vector<DBEntry<IHeader>> entries({ BlockDB::ToHeaderEntry(pHeader) });
    m_pDatabase->Put(HEADER_TABLE, entries);
}

void BlockDB::AddHeaders(const std::vector<IHeader::CPtr>& headers)
{
    LOG_TRACE_F("Adding {} headers", headers.size());

    std::vector<DBEntry<IHeader>> entries;
    entries.reserve(headers.size());

    std::transform(
        headers.cbegin(), headers.cend(),
        std::back_inserter(entries),
        BlockDB::ToHeaderEntry
    );
    m_pDatabase->Put(HEADER_TABLE, entries);
}

IBlock::CPtr BlockDB::GetBlockByHash(const Hash& hash) const noexcept
{
    LOG_TRACE_F("Loading block {}", hash);

    auto pEntry = m_pDatabase->Get<IBlock>(BLOCK_TABLE, hash.ToHex());
    if (pEntry != nullptr)
    {
        LOG_DEBUG_F("IBlock found for hash {}", hash);
        return pEntry->item;
    }
    else
    {
        LOG_DEBUG_F("No block found for hash {}", hash);
        return nullptr;
    }
}

IBlock::CPtr BlockDB::GetBlockByHeight(const uint64_t height) const noexcept
{
    LOG_TRACE_F("Loading block {}", height);

    tl::optional<Hash> hashOpt = m_pChainStore->GetHashByHeight(height);
    if (hashOpt.has_value())
    {
        LOG_DEBUG_F("Hash {} found for height {}", hashOpt.value(), height);
        return GetBlockByHash(hashOpt.value());
    }
    else
    {
        LOG_DEBUG_F("No hash found at height {}", height);
        return nullptr;
    }
}

void BlockDB::AddBlock(const IBlock::CPtr& pBlock)
{
    LOG_TRACE_F("Saving block {}", pBlock);

    std::vector<DBEntry<IBlock>> entries({ BlockDB::ToBlockEntry(pBlock) });
    m_pDatabase->Put<IBlock>(BLOCK_TABLE, entries);
}