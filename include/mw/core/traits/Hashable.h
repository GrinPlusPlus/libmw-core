#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/models/crypto/Hash.h>

namespace Traits
{
    class IHashable
    {
    public:
        virtual ~IHashable() = default;

        virtual Hash GetHash() const noexcept = 0;
    };
}