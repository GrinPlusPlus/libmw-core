#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/models/crypto/BigInteger.h>
#include <mw/traits/Printable.h>
#include <mw/traits/Serializable.h>
#include <mw/util/BitUtil.h>

#include <cassert>

class Commitment : public Traits::IPrintable, public Traits::ISerializable
{
public:
    using SIZE = 33;

    //
    // Constructors
    //
    Commitment() = default;
    Commitment(BigInt<SIZE>&& bytes) : m_bytes(std::move(bytes)) { assert(m_bytes.size() == SIZE); }
    Commitment(const BigInt<33>& bytes) : m_bytes(bytes) { }
    Commitment(const Commitment& other) = default;
    Commitment(Commitment&& other) noexcept = default;

    //
    // Destructor
    //
    virtual ~Commitment() = default;

    //
    // Operators
    //
    Commitment& operator=(const Commitment& other) = default;
    Commitment& operator=(Commitment&& other) noexcept = default;
    bool operator<(const Commitment& rhs) const { return m_bytes < rhs.GetBigInt(); }
    bool operator!=(const Commitment& rhs) const { return m_bytes != rhs.GetBigInt(); }
    bool operator==(const Commitment& rhs) const { return m_bytes == rhs.GetBigInt(); }

    //
    // Getters
    //
    const BigInt<SIZE>& GetBigInt() const { return m_bytes; }
    const std::vector<uint8_t>& GetVec() const { return m_bytes.vec(); }
    const uint8_t* data() const { return m_bytes.data(); }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        return m_bytes.Serialize(serializer);
    }

    static Commitment Deserialize(ByteBuffer& byteBuffer)
    {
        return Commitment(BigInt<32>::Deserialize(SIZE));
    }

    //
    // Traits
    //
    virtual std::string Format() const override final { return m_bytes.Format(); }

private:
    // The BigInt representation of the Commitment
    BigInt<SIZE> m_bytes;
};

namespace std
{
    template<>
    struct hash<Commitment>
    {
        size_t operator()(const Commitment& commitment) const
        {
            const std::vector<uint8_t>& bytes = commitment.GetVec();
            return BitUtil::ConvertToU64(bytes[0], bytes[4], bytes[8], bytes[12], bytes[16], bytes[20], bytes[24], bytes[28]);
        }
    };
}