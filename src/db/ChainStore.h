#pragma once

#include <mw/core/models/block/Header.h>
#include <mw/core/file/FilePath.h>

class ChainStore
{
public:
    using Ptr = std::shared_ptr<ChainStore>;

    static ChainStore::Ptr Load(const FilePath& chainPath);

    tl::optional<Hash> GetHashByHeight(const uint64_t height) const noexcept;
};