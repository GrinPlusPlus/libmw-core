#pragma once

#include <mw/core/models/block/IHeader.h>
#include <mw/core/models/block/IBlock.h>
#include <mw/core/traits/Batchable.h>
#include <mw/core/common/Lock.h>
#include <mw/core/file/FilePath.h>

// TODO: GetHeaderByHeight, GetHeaderByHeightRange, GetBlockByHeight, GetBlockByHeightRange, GetBlockByCommitment(if indexed)
class IBlockDB : public Traits::IBatchable
{
public:
    virtual ~IBlockDB() = default;

	//
	// Retrieve Headers
	//
	virtual IHeader::CPtr GetHeaderByHash(const Hash& hash) const noexcept = 0;
	virtual std::vector<IHeader::CPtr> GetHeadersByHash(const std::vector<Hash>& hashes) const noexcept = 0;

	//
	// Save Headers
	//
	virtual void AddHeader(const IHeader::CPtr& pHeader) = 0;
	virtual void AddHeaders(const std::vector<IHeader::CPtr>& headers) = 0;

	//
	// Retrieve Blocks
	//
	virtual IBlock::CPtr GetBlockByHash(const Hash& hash) const noexcept = 0;
	virtual IBlock::CPtr GetBlockByHeight(const uint64_t height) const noexcept = 0;

	//
	// Save Blocks
	//
	virtual void AddBlock(const IBlock::CPtr& pBlock) = 0;
};

class BlockDBFactory
{
public:
	static Locked<IBlockDB> Open(const Context::CPtr& pContext, const FilePath& chainPath);
};