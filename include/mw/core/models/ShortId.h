#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/models/crypto/BigInteger.h>
#include <mw/core/traits/Printable.h>
#include <mw/core/traits/Serializable.h>
#include <mw/core/traits/Hashable.h>

class ShortId
{
public:
    //
    // Constructors
    //
    ShortId(BigInt<6>&& id) : m_id(std::move(id)) { }
    ShortId(const ShortId& other) = default;
    ShortId(ShortId&& other) noexcept = default;
    ShortId() = default;
    static ShortId Create(const BigInt<32>& hash, const BigInt<32>& blockHash, const uint64_t nonce);

    //
    // Destructor
    //
    virtual ~ShortId() = default;

    //
    // Operators
    //
    ShortId& operator=(const ShortId& other) = default;
    ShortId& operator=(ShortId&& other) noexcept = default;
    bool operator<(const ShortId& shortId) const { return GetHash() < shortId.GetHash(); }

    //
    // Getters
    //
    const BigInt<6>& GetId() const { return m_id; }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        return m_id.Serialize(serializer);
    }

    static ShortId Deserialize(ByteBuffer& byteBuffer);

    //
    // Hashing
    //
    Hash GetHash() const;

private:
    BigInt<6> m_id;
};