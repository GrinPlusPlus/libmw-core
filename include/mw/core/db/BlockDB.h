#pragma once

#include <mw/core/models/block/Header.h>
#include <mw/core/models/block/Block.h>
#include <mw/core/traits/Batchable.h>

// TODO: GetHeaderByHeight, GetHeaderByHeightRange, GetBlockByHeight, GetBlockByHeightRange, GetBlockByCommitment(if indexed)
class IBlockDB : public Traits::IBatchable
{
public:
    virtual IBlockDB() = default;

	//
	// Retrieve Headers
	//
	virtual IHeader::CPtr GetHeaderByHash(const Hash& hash) const = 0;
	virtual std::vector<IHeader::CPtr> GetHeadersByHash(const std::vector<Hash>& hashes) const = 0;

	//
	// Save Headers
	//
	virtual void AddHeader(IHeader::CPtr pHeader) = 0;
	virtual void AddHeaders(const std::vector<IHeader::CPtr>& headers) = 0;

	//
	// Retrieve Blocks
	//
	virtual Block::CPtr GetBlockByHash(const Hash& hash) const = 0;
	virtual Block::CPtr GetBlockByHeight(const uint64_t height) const = 0;

	//
	// Save Blocks
	//
	virtual void AddBlock(const Block& block) = 0;
};