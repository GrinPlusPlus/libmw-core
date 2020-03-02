#pragma once

#include <mw/core/models/crypto/BigInteger.h>
#include <mw/core/traits/Printable.h>
#include <mw/core/traits/Serializable.h>

class PublicKey : public Traits::IPrintable, public Traits::ISerializable
{
public:
    PublicKey() = default;
    PublicKey(BigInt<33>&& compressed) : m_compressed(std::move(compressed)) { }
    virtual ~PublicKey() = default;

    const BigInt<33>& GetBigInt() const { return m_compressed; }
    const std::vector<uint8_t>& vec() const { return m_compressed.vec(); }
    const uint8_t* data() const { return m_compressed.data(); }
    uint8_t* data() { return m_compressed.data(); }
    size_t size() const { return m_compressed.size(); }

    Serializer& Serialize(Serializer& serializer) const noexcept final { return m_compressed.Serialize(serializer); }
    static PublicKey Deserialize(Deserializer& deserializer) { return BigInt<33>::Deserialize(deserializer); }

    std::string Format() const final { return m_compressed.ToHex(); }

private:
    BigInt<33> m_compressed;
};