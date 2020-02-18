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
	// Retrieve Headers
	//
	virtual IHeader::CPtr GetHeaderByHash(const Hash& hash) const noexcept override final;
	virtual std::vector<IHeader::CPtr> GetHeadersByHash(const std::vector<Hash>& hashes) const noexcept override final;

	//
	// Save Headers
	//
	virtual void AddHeader(const IHeader::CPtr& pHeader) override final;
	virtual void AddHeaders(const std::vector<IHeader::CPtr>& headers) override final;

	//
	// Retrieve Blocks
	//
	virtual Block::CPtr GetBlockByHash(const Hash& hash) const noexcept override final;
	virtual Block::CPtr GetBlockByHeight(const uint64_t height) const noexcept override final;

	//
	// Save Blocks
	//
	virtual void AddBlock(const Block::CPtr& pBlock) override final;

	virtual void Commit() override final { return m_pDatabase->Commit(); }
	virtual void Rollback() noexcept override final { return m_pDatabase->Rollback(); }
	virtual void OnInitWrite() noexcept override final { m_pDatabase->OnInitWrite(); }
	virtual void OnEndWrite() noexcept override final { m_pDatabase->OnEndWrite(); }

private:
	static DBEntry<IHeader> ToHeaderEntry(const IHeader::CPtr& pHeader)
	{
		return DBEntry<IHeader>(pHeader->GetHash().ToHex(), pHeader);
	}

	static DBEntry<Block> ToBlockEntry(const Block::CPtr& pBlock)
	{
		return DBEntry<Block>(pBlock->GetHash().ToHex(), pBlock);
	}

	Database::Ptr m_pDatabase;
	ChainStore::Ptr m_pChainStore;
};