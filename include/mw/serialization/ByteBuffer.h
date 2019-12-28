#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/util/EndianUtil.h>
#include <mw/exceptions/DeserializationException.h>

#include <vector>
#include <string>
#include <cstring>
#include <cstdint>
#include <algorithm>

class ByteBuffer
{
public:
    ByteBuffer(const std::vector<uint8_t>& bytes) : m_index(0), m_bytes(bytes) { }
    ByteBuffer(std::vector<uint8_t>&& bytes) : m_index(0), m_bytes(std::move(bytes)) { }

    template<class T>
    void ReadBigEndian(T& t)
    {
        if (m_index + sizeof(T) > m_bytes.size())
        {
            throw DeserializationEx("Attempted to read past end of ByteBuffer.");
        }

        if (EndianUtil::IsBigEndian())
        {
            memcpy(&t, &m_bytes[m_index], sizeof(T));
        }
        else
        {
            std::vector<uint8_t> temp;
            temp.resize(sizeof(T));
            std::reverse_copy(m_bytes.cbegin() + m_index, m_bytes.cbegin() + m_index + sizeof(T), temp.begin());
            memcpy(&t, &temp[0], sizeof(T));
        }

        m_index += sizeof(T);
    }

    template<class T>
    void ReadLittleEndian(T& t)
    {
        if (m_index + sizeof(T) > m_bytes.size())
        {
            throw DeserializationEx("Attempted to read past end of ByteBuffer.");
        }

        if (EndianUtil::IsBigEndian())
        {
            std::vector<uint8_t> temp;
            temp.resize(sizeof(T));
            std::reverse_copy(m_bytes.cbegin() + m_index, m_bytes.cbegin() + m_index + sizeof(T), temp.begin());
            memcpy(&t, &temp[0], sizeof(T));
        }
        else
        {
            memcpy(&t, &m_bytes[m_index], sizeof(T));
        }

        m_index += sizeof(T);
    }

    int8_t Read8()
    {
        int8_t value;
        ReadBigEndian(value);

        return value;
    }

    uint8_t ReadU8()
    {
        uint8_t value;
        ReadBigEndian(value);

        return value;
    }

    int16_t Read16()
    {
        int16_t value;
        ReadBigEndian(value);

        return value;
    }

    uint16_t ReadU16()
    {
        uint16_t value;
        ReadBigEndian(value);

        return value;
    }

    int32_t Read32()
    {
        int32_t value;
        ReadBigEndian(value);

        return value;
    }

    uint32_t ReadU32()
    {
        uint32_t value;
        ReadBigEndian(value);

        return value;
    }

    int64_t Read64()
    {
        int64_t value;
        ReadBigEndian(value);

        return value;
    }

    uint64_t ReadU64()
    {
        uint64_t value;
        ReadBigEndian(value);

        return value;
    }

    uint64_t ReadU64_LE()
    {
        uint64_t value;
        ReadLittleEndian(value);

        return value;
    }

    std::string ReadVarStr()
    {
        const uint64_t stringLength = ReadU64();
        if (stringLength == 0)
        {
            return "";
        }

        if (m_index + stringLength > m_bytes.size())
        {
            throw DeserializationEx("Attempted to read past end of ByteBuffer.");
        }

        std::vector<uint8_t> temp(m_bytes.cbegin() + m_index, m_bytes.cbegin() + m_index + stringLength);
        m_index += stringLength;

        return std::string((char*)&temp[0], stringLength);
    }

    std::vector<uint8_t> ReadVector(const uint64_t numBytes)
    {
        if (m_index + numBytes > m_bytes.size())
        {
            throw DeserializationEx("Attempted to read past end of ByteBuffer.");
        }

        const size_t index = m_index;
        m_index += numBytes;

        return std::vector<uint8_t>(m_bytes.cbegin() + index, m_bytes.cbegin() + index + numBytes);
    }

    template<size_t T>
    std::array<uint8_t, T> ReadArray()
    {
        if (m_index + T > m_bytes.size())
        {
            throw DeserializationEx("Attempted to read past end of ByteBuffer.");
        }

        const size_t index = m_index;
        m_index += T;

        std::array<uint8_t, T> arr;
        std::copy(m_bytes.begin() + index, m_bytes.begin() + index + T, arr.begin());
        return arr;
    }

    size_t GetRemainingSize() const
    {
        return m_bytes.size() - m_index;
    }

private:
    size_t m_index;
    const std::vector<uint8_t>& m_bytes;
};
