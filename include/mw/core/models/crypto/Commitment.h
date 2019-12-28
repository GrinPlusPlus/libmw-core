#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/models/crypto/BigInteger.h>
#include <mw/core/traits/Printable.h>
#include <mw/core/traits/Serializable.h>
#include <mw/core/traits/Jsonable.h>
#include <mw/core/util/BitUtil.h>

#include <cassert>

class Commitment :
    public Traits::IPrintable,
    public Traits::ISerializable,
    public Traits::IJsonable
{
public:
    static constexpr size_t const& SIZE = 33;

    //
    // Constructors
    //
    Commitment() = default;
    Commitment(BigInt<SIZE>&& bytes) : m_bytes(std::move(bytes)) { assert(m_bytes.size() == SIZE); }
    Commitment(const BigInt<SIZE>& bytes) : m_bytes(bytes) { }
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
    uint8_t* data() { return m_bytes.data(); }
    size_t size() const { return m_bytes.size(); }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        return m_bytes.Serialize(serializer);
    }

    static Commitment Deserialize(ByteBuffer& byteBuffer)
    {
        return Commitment(BigInt<SIZE>::Deserialize(byteBuffer));
    }

    virtual json ToJSON() const override final
    {
        return json(m_bytes.ToHex());
    }

    static Commitment FromJSON(const json& json)
    {
        return Commitment::FromHex(json.get<std::string>());
    }

    std::string ToHex() const { return m_bytes.ToHex(); }
    static Commitment FromHex(const std::string& hex) { return Commitment(BigInt<SIZE>::FromHex(hex)); }

    //
    // Traits
    //
    virtual std::string Format() const override final { return m_bytes.Format(); }

private:
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