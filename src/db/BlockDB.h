#pragma once

#include <mw/core/db/IBlockDB.h>

#include "common/Database.h"
#include "ChainStore.h"

class BlockDB : public IBlockDB
{
public:
    BlockDB(const Database::Ptr& pDatabase, const ChainStore::Ptr& pChainStore)
        : m_pDatabase(pDatabase), m_pChainStore(pChainStore) { }

    //
    // Headers
    //
    IHeader::CPtr GetHeaderByHash(const Hash& hash) const noexcept final;
    std::vector<IHeader::CPtr> GetHeadersByHash(const std::vector<Hash>& hashes) const noexcept final;
    void AddHeader(const IHeader::CPtr& pHeader) final;
    void AddHeaders(const std::vector<IHeader::CPtr>& headers) final;

    //
    // Blocks
    //
    IBlock::CPtr GetBlockByHash(const Hash& hash) const noexcept final;
    IBlock::CPtr GetBlockByHeight(const uint64_t height) const noexcept final;
    void AddBlock(const IBlock::CPtr& pBlock) final;
    void RemoveOldBlocks(const uint64_t height) final;

    //
    // UTXOs
    //
    std::unordered_map<Commitment, UTXO::CPtr> GetUTXOs(const std::vector<Commitment>& commitments) const noexcept final;
    void AddUTXOs(const std::vector<UTXO::CPtr>& utxos) final;
    void RemoveUTXOs(const std::vector<Commitment>& commitment) final;
    void RemoveAllUTXOs() final;

    void Commit() final { return m_pDatabase->Commit(); }
    void Rollback() noexcept final { return m_pDatabase->Rollback(); }
    void OnInitWrite() noexcept final { m_pDatabase->OnInitWrite(); }
    void OnEndWrite() noexcept final { m_pDatabase->OnEndWrite(); }

private:
    static DBEntry<IHeader> ToHeaderEntry(const IHeader::CPtr& pHeader)
    {
        return DBEntry<IHeader>(pHeader->GetHash().ToHex(), pHeader);
    }

    static DBEntry<IBlock> ToBlockEntry(const IBlock::CPtr& pBlock)
    {
        return DBEntry<IBlock>(pBlock->GetHash().ToHex(), pBlock);
    }

    Database::Ptr m_pDatabase;
    ChainStore::Ptr m_pChainStore;
};