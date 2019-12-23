#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/models/crypto/BigInteger.h>
#include <mw/traits/Serializable.h>

class Signature : public Traits::ISerializable
{
public:
    using SIZE = 64;

    //
    // Constructors
    //
    Signature(BigInt<SIZE>&& bytes) : m_bytes(std::move(bytes)) { }
    Signature(const Signature& other) = default;
    Signature(Signature&& other) noexcept = default;

    //
    // Destructor
    //
    virtual ~Signature() = default;

    //
    // Operators
    //
    Signature& operator=(const Signature& other) = default;
    Signature& operator=(Signature&& other) noexcept = default;

    //
    // Getters
    //
    const BigInt<SIZE>& GetBigInt() const { return m_bytes; }

    //
    // Serialization/Deserialization
    //
    void Serialize(Serializer& serializer) const
    {
        m_bytes.Serialize(serializer);
    }

    static Signature Deserialize(ByteBuffer& byteBuffer)
    {
        return Signature(BigInt<SIZE>::Deserialize(byteBuffer));
    }

    std::string ToHex() const { return m_bytes.ToHex(); }

private:
    // The 64 byte Signature.
    BigInt<SIZE> m_bytes;
};

class CompactSignature : public Signature
{
public:
    CompactSignature(BigInt<SIZE>&& bytes) : Signature(std::move(bytes)) { }

    virtual ~CompactSignature() = default;
};

// TODO: Create RawSignature