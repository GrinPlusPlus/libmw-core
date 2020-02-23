#pragma once

#include <mw/core/models/block/IHeader.h>
#include <mw/core/file/FilePath.h>
#include <tl/optional.hpp>

class ChainStore
{
public:
    using Ptr = std::shared_ptr<ChainStore>;

    static ChainStore::Ptr Load(const FilePath& chainPath);

    tl::optional<Hash> GetHashByHeight(const uint64_t height) const noexcept;
};