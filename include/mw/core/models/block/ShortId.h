#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/Context.h>
#include <mw/core/models/crypto/BigInteger.h>
#include <mw/core/traits/Printable.h>
#include <mw/core/traits/Serializable.h>
#include <mw/core/traits/Hashable.h>
#include <mw/core/crypto/Crypto.h>

class ShortId :
    public Traits::IPrintable,
    public Traits::ISerializable,
    public Traits::IHashable
{
public:
    //
    // Constructors
    //
    ShortId(BigInt<6>&& id) : m_id(std::move(id)) { }
    ShortId(const ShortId& other) = default;
    ShortId(ShortId&& other) noexcept = default;
    ShortId() = default;

    static ShortId Create(const BigInt<32>& hash, const BigInt<32>& blockHash, const uint64_t nonce)
    {
        // take the block hash and the nonce and hash them together
        Serializer serializer;
        blockHash.Serialize(serializer);
        serializer.Append<uint64_t>(nonce);
        const BigInt<32> hashWithNonce = Crypto::Blake2b(serializer.vec());

        // extract k0/k1 from the block_hash
        Deserializer deserializer(hashWithNonce.vec());
        const uint64_t k0 = deserializer.ReadU64_LE();
        const uint64_t k1 = deserializer.ReadU64_LE();

        // SipHash24 our hash using the k0 and k1 keys
        const uint64_t sipHash = Crypto::SipHash24(k0, k1, hash.vec());

        // construct a short_id from the resulting bytes (dropping the 2 most significant bytes)
        Serializer serializer2;
        serializer2.AppendLE<uint64_t>(sipHash);

        return ShortId(BigInt<6>(serializer2.data()));
    }

    //
    // Destructor
    //
    virtual ~ShortId() = default;

    //
    // Operators
    //
    ShortId& operator=(const ShortId& other) = default;
    ShortId& operator=(ShortId&& other) noexcept = default;
    bool operator<(const ShortId& shortId) const noexcept { return GetHash() < shortId.GetHash(); }

    //
    // Getters
    //
    const BigInt<6>& GetId() const noexcept { return m_id; }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const noexcept override final { return m_id.Serialize(serializer); }
    static ShortId Deserialize(const Context::CPtr&, Deserializer& deserializer) { return BigInt<6>::Deserialize(deserializer); }

    //
    // Traits
    //
    virtual std::string Format() const override final { return m_id.Format(); }
    virtual Hash GetHash() const noexcept override final { return Crypto::Blake2b(m_id.vec()); }

private:
    BigInt<6> m_id;
};