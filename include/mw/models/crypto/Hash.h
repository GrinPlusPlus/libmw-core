#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/models/crypto/BigInteger.h>
#include <mw/util/BitUtil.h>

using Hash = BigInt<32>;

class HASH
{
public:
    static inline const Hash ZERO = Hash::ValueOf(0);
};

#define ZERO_HASH HASH::ZERO

namespace std
{
    template<>
    struct hash<Hash>
    {
        size_t operator()(const Hash& hash) const
        {
            return BitUtil::ConvertToU64(hash[0], hash[4], hash[8], hash[12], hash[16], hash[20], hash[24], hash[28]);
        }
    };
}