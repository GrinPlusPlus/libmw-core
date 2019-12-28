#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <memory>

namespace Traits
{
    class IBatchable
    {
    public:
        IBatchable() : m_dirty(false) { }
        virtual ~IBatchable() = default;

        virtual void Commit() = 0;
        virtual void Rollback() = 0;

        // This can be overridden
        virtual void OnInitWrite() { }

        // This can be overridden
        virtual void OnEndWrite() { }

        bool IsDirty() const { return m_dirty; }
        void SetDirty(const bool dirty) { m_dirty = dirty; }

    private:
        bool m_dirty;
    };
}