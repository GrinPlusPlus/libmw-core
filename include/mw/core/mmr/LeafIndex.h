#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/mmr/Index.h>

namespace mmr
{
class LeafIndex
{
public:
    LeafIndex(const uint64_t leafIndex, const uint64_t position)
        : m_leafIndex(leafIndex), m_nodeIndex(position, 0) { }
    virtual ~LeafIndex() = default;

    static LeafIndex At(const uint64_t leafIndex) noexcept
    {
        return LeafIndex(leafIndex, (2 * leafIndex) - BitUtil::CountBitsSet(leafIndex));
    }

    uint64_t GetLeafIndex() const noexcept { return m_leafIndex; }
    const Index& GetNodeIndex() const noexcept { return m_nodeIndex; }
    uint64_t GetPosition() const noexcept { return m_nodeIndex.GetPosition(); }

private:
    uint64_t m_leafIndex;
    Index m_nodeIndex;
};
}