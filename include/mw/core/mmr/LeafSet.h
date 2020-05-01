#pragma once

#include <mw/core/Context.h>
#include <mw/core/file/File.h>
#include <mw/core/models/crypto/Hash.h>
#include <mw/core/mmr/LeafIndex.h>
#include <mw/core/traits/Batchable.h>

namespace mmr
{
    class LeafSet : public Traits::IBatchable
    {
    public:
        using Ptr = std::shared_ptr<LeafSet>;

        static LeafSet::Ptr Load(const Context::CPtr& pContext);
        virtual ~LeafSet() = default;

        void Add(const LeafIndex& idx);
        void Remove(const LeafIndex& idx);
        bool Contains(const LeafIndex& idx) const noexcept;
        Hash Root(const uint64_t numLeaves) const;
        uint64_t GetSize() const;

        void Rewind(const uint64_t numLeaves, const std::vector<LeafIndex>& leavesToAdd);
        void Commit() final;
        void Rollback() noexcept final;
        void Snapshot(const File& snapshotFile) const;
    };
}