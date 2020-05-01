#pragma once

#include <mw/core/models/block/IHeader.h>
#include <mw/core/models/block/IBlock.h>
#include <mw/core/models/tx/UTXO.h>
#include <mw/core/traits/Batchable.h>
#include <mw/core/common/Lock.h>
#include <mw/core/file/FilePath.h>

// TODO: GetHeaderByHeight, GetHeaderByHeightRange, GetBlockByHeightRange, GetBlockByCommitment(if indexed)
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

	//
	// Removes all blocks before the given height.
	// This is most useful when pruning/compacting the chain.
	//
	virtual void RemoveOldBlocks(const uint64_t height) = 0;

	//
	// Retrieve UTXOs for the given commitments.
	// If there are multiple UTXOs for a commitment, the most recent will be returned.
	//
	virtual std::unordered_map<Commitment, UTXO::CPtr> GetUTXOs(
		const std::vector<Commitment>& commitments
	) const noexcept = 0;

	//
	// Add the UTXOs
	//
	virtual void AddUTXOs(const std::vector<UTXO::CPtr>& utxos) = 0;

	//
	// Removes the UTXOs for the given commitments.
	// If there are multiple UTXOs for a commitment, the most recent will be removed.
	// DatabaseException thrown if no UTXOs are found fo a commitment.
	//
	virtual void RemoveUTXOs(const std::vector<Commitment>& commitment) = 0;

	//
	// Removes all of the UTXOs from the database.
	// This is useful when resyncing the chain.
	//
	virtual void RemoveAllUTXOs() = 0;


};

class BlockDBFactory
{
public:
	static Locked<IBlockDB> Open(const Context::CPtr& pContext, const FilePath& chainPath);
};