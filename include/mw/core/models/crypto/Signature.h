#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/models/crypto/BigInteger.h>
#include <mw/core/traits/Serializable.h>

class Signature : public Traits::ISerializable
{
public:
    using UPtr = std::unique_ptr<const Signature>;
    static constexpr size_t const& SIZE = 64;

    //
    // Constructors
    //
    Signature() = default;
    Signature(const uint8_t* data) : m_bytes(data) { }
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
    const uint8_t* data() const { return m_bytes.data(); }
    uint8_t* data() { return m_bytes.data(); }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        return m_bytes.Serialize(serializer);
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
    using UPtr = std::unique_ptr<const CompactSignature>;

    CompactSignature() = default;
    CompactSignature(BigInt<SIZE>&& bytes) : Signature(std::move(bytes)) { }

    virtual ~CompactSignature() = default;
};

// TODO: Create RawSignature