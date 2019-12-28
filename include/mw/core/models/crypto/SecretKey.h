#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/common/Secure.h>
#include <mw/core/models/crypto/BigInteger.h>
#include <mw/core/traits/Serializable.h>

template<size_t NUM_BYTES>
class secret_key_t : public Traits::ISerializable
{
public:
    //
    // Constructor
    //
    secret_key_t() = default;
    secret_key_t(BigInt<NUM_BYTES>&& value) : m_value(std::move(value)) { }
    secret_key_t(const SecureVector& bytes) : m_value(BigInt<NUM_BYTES>(bytes.data())) { }
    secret_key_t(std::vector<uint8_t>&& bytes) : m_value(BigInt<NUM_BYTES>(std::move(bytes))) { }

    //
    // Destructor
    //
    virtual ~secret_key_t() = default;

    //
    // Getters
    //
    const BigInt<NUM_BYTES>& GetBigInt() const { return m_value; }
    const std::vector<uint8_t>& vec() const { return m_value.vec(); }
    uint8_t* data() { return m_value.data(); }
    const uint8_t* data() const { return m_value.data(); }
    size_t size() const { return m_value.size(); }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        return m_value.Serialize(serializer);
    }

    static secret_key_t<NUM_BYTES> Deserialize(ByteBuffer& byteBuffer)
    {
        return secret_key_t<NUM_BYTES>(BigInt<NUM_BYTES>::Deserialize(byteBuffer));
    }

private:
    BigInt<NUM_BYTES> m_value; // TODO: Use SecureAllocator
};

using SecretKey = secret_key_t<32>;
using SecretKey64 = secret_key_t<64>;