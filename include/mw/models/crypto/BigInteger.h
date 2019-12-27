#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/util/BitUtil.h>
#include <mw/util/HexUtil.h>
#include <mw/traits/Printable.h>
#include <mw/traits/Serializable.h>

#include <cassert>
#include <cstdint>
#include <vector>
#include <string>
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <algorithm>

#pragma warning(disable: 4505)

template<size_t NUM_BYTES, class ALLOC = std::allocator<uint8_t>>
class BigInt : public Traits::IPrintable, public Traits::ISerializable
{
public:
    //
    // Constructors
    //
    BigInt() : m_bytes(NUM_BYTES) { }
    BigInt(const std::vector<uint8_t, ALLOC>& bytes) : m_bytes(bytes) { assert(bytes.size() == NUM_BYTES); }
    BigInt(std::vector<uint8_t, ALLOC>&& bytes) : m_bytes(std::move(bytes)) { assert(bytes.size() == NUM_BYTES); }
    BigInt(const uint8_t* arr) : m_bytes(arr, arr + NUM_BYTES) { }
    BigInt(const BigInt& bigInteger) = default;
    BigInt(BigInt&& bigInteger) noexcept = default;

    //
    // Destructor
    //
    virtual ~BigInt()
    {
        for (size_t i = 0; i < m_bytes.size(); i++)
        {
            m_bytes[i] = 0;
        }
    }

    size_t size() const { return NUM_BYTES; }
    const std::vector<uint8_t, ALLOC>& vec() const { return m_bytes; }
    uint8_t* data() { return m_bytes.data(); }
    const uint8_t* data() const { return m_bytes.data(); }

    // TODO: Take in uint64_t
    static BigInt<NUM_BYTES, ALLOC> ValueOf(const uint8_t value)
    {
        std::vector<uint8_t, ALLOC> bytes(NUM_BYTES);
        bytes[NUM_BYTES - 1] = value;
        return BigInt<NUM_BYTES, ALLOC>(std::move(bytes));
    }

    static BigInt<NUM_BYTES, ALLOC> FromHex(const std::string& hex)
    {
        assert(hex.length() == NUM_BYTES * 2);
        std::vector<uint8_t> bytes = HexUtil::FromHex(hex);
        assert(bytes.size() == NUM_BYTES);
        return BigInt<NUM_BYTES, ALLOC>(std::move(bytes));
    }

    static BigInt<NUM_BYTES, ALLOC> Max()
    {
        std::vector<uint8_t, ALLOC> bytes(NUM_BYTES);
        for (int i = 0; i < NUM_BYTES; i++)
        {
            bytes[i] = 0xFF;
        }

        return BigInt<NUM_BYTES, ALLOC>(std::move(bytes));
    }

    std::string ToHex() const { return HexUtil::ToHex(m_bytes); }
    virtual std::string Format() const override final { return ToHex(); }

    //
    // Operators
    //
    BigInt& operator=(const BigInt& other) = default;
    BigInt& operator=(BigInt&& other) noexcept = default;

    BigInt operator^(const BigInt& rhs) const
    {
        BigInt<NUM_BYTES, ALLOC> result = *this;
        for (size_t i = 0; i < NUM_BYTES; i++)
        {
            result[i] ^= rhs[i];
        }

        return result;
    }

    uint8_t& operator[] (const size_t x) { return m_bytes[x]; }
    const uint8_t& operator[] (const size_t x) const { return m_bytes[x]; }

    bool operator<(const BigInt& rhs) const
    {
        if (this == &rhs)
        {
            return false;
        }

        assert(m_bytes.size() == NUM_BYTES && rhs.m_bytes.size() == NUM_BYTES);
        for (size_t i = 0; i < NUM_BYTES; i++)
        {
            if (m_bytes[i] != rhs.m_bytes[i])
            {
                return m_bytes[i] < rhs.m_bytes[i];
            }
        }

        return false;
    }

    bool operator>(const BigInt& rhs) const
    {
        return rhs < *this;
    }

    bool operator==(const BigInt& rhs) const
    {
        return this == &rhs || this->m_bytes == rhs.m_bytes;
    }

    bool operator!=(const BigInt& rhs) const
    {
        return !(*this == rhs);
    }

    bool operator<=(const BigInt& rhs) const
    {
        return *this < rhs || *this == rhs;
    }

    bool operator>=(const BigInt& rhs) const
    {
        return *this > rhs || *this == rhs;
    }

    BigInt operator^=(const BigInt& rhs)
    {
        *this = *this ^ rhs;

        return *this;
    }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        return serializer.AppendByteVector(m_bytes);
    }

    static BigInt<NUM_BYTES, ALLOC> Deserialize(ByteBuffer& byteBuffer)
    {
        return BigInt<NUM_BYTES, ALLOC>(byteBuffer.ReadVector(NUM_BYTES));
    }

#ifdef INCLUDE_TEST_MATH
    // Not safe for use in production code
    BigInt operator/(const int divisor) const
    {
        std::vector<uint8_t, ALLOC> quotient(NUM_BYTES);

        int remainder = 0;
        for (int i = 0; i < NUM_BYTES; i++)
        {
            remainder = remainder * 256 + m_bytes[i];
            quotient[i] = (uint8_t)(remainder / divisor);
            remainder -= quotient[i] * divisor;
        }

        return BigInt<NUM_BYTES, ALLOC>(std::move(quotient));
    }
#endif

private:
    std::vector<uint8_t, ALLOC> m_bytes;
};