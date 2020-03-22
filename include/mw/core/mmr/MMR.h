#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/models/crypto/Hash.h>
#include <mw/core/util/BitUtil.h>
#include <mw/core/traits/Batchable.h>
#include <mw/core/file/FilePath.h>
#include <mw/core/mmr/Backend.h>
#include <mw/core/mmr/LeafIndex.h>
#include <mw/core/mmr/Leaf.h>
#include <mw/core/mmr/Node.h>

namespace mmr
{
class MMR : public Traits::IBatchable
{
public:
    using Ptr = std::shared_ptr<MMR>;
    using CPtr = std::shared_ptr<const MMR>;

    MMR(const IBackend::Ptr& pBackend) : m_pBackend(pBackend) { }

    void Add(std::vector<uint8_t>&& data);
    void Add(const std::vector<uint8_t>& data) { return Add(std::vector<uint8_t>(data)); }
    Leaf Get(const LeafIndex& leafIdx) const { return m_pBackend->GetLeaf(leafIdx); }

    uint64_t GetNumNodes() const noexcept;
    void Rewind(const uint64_t numNodes);

    //
    // Unlike a Merkle tree, a MMR generally has no single root so we need a method to compute one.
    // The process we use is called "bagging the peaks." We first identify the peaks (nodes with no parents).
    // We then "bag" them by hashing them iteratively from the right, using the total size of the MMR as prefix. 
    //
    Hash Root() const;

    void Commit() final { m_pBackend->Commit(); }
    void Rollback() noexcept final { m_pBackend->Rollback(); }

private:
    IBackend::Ptr m_pBackend;
};
}