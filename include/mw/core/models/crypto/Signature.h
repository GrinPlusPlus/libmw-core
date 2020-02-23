#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/models/crypto/BigInteger.h>
#include <mw/core/traits/Printable.h>
#include <mw/core/traits/Serializable.h>
#include <mw/core/traits/Jsonable.h>

class Signature :
    public Traits::IPrintable,
    public Traits::ISerializable,
    public Traits::IJsonable
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
    Signature(const BigInt<SIZE>& bytes) : m_bytes(bytes) { }
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
    bool operator<(const Signature& rhs) const { return m_bytes < rhs.GetBigInt(); }
    bool operator!=(const Signature& rhs) const { return m_bytes != rhs.GetBigInt(); }
    bool operator==(const Signature& rhs) const { return m_bytes == rhs.GetBigInt(); }

    //
    // Getters
    //
    const BigInt<SIZE>& GetBigInt() const { return m_bytes; }
    const uint8_t* data() const { return m_bytes.data(); }
    uint8_t* data() { return m_bytes.data(); }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const noexcept override final
    {
        return m_bytes.Serialize(serializer);
    }

    static Signature Deserialize(Deserializer& deserializer)
    {
        return Signature(BigInt<SIZE>::Deserialize(deserializer));
    }

    virtual json ToJSON() const noexcept override final
    {
        return json(m_bytes.ToHex());
    }

    static Signature FromJSON(const Json& json)
    {
        return Signature::FromHex(json.Get<std::string>());
    }

    std::string ToHex() const { return m_bytes.ToHex(); }
    static Signature FromHex(const std::string& hex) { return Signature(BigInt<SIZE>::FromHex(hex)); }

    //
    // Traits
    //
    virtual std::string Format() const override final { return m_bytes.Format(); }

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