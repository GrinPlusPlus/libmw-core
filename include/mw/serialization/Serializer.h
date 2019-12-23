#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

//#include <mw/Secure.h>
#include <mw/util/EndianUtil.h>

#include <cstdint>
#include <vector>
#include <string>
#include <algorithm>

class Serializer
{
public:
    Serializer() = default;
    Serializer(const size_t expectedSize) { m_serialized.reserve(expectedSize); }

    template <class T>
    Serializer& Append(const T& t)
    {
        std::vector<uint8_t> temp(sizeof(T));
        memcpy(&temp[0], &t, sizeof(T));

        if (EndianUtil::IsBigEndian())
        {
            m_serialized.insert(m_serialized.end(), temp.cbegin(), temp.cend());
        }
        else
        {
            m_serialized.insert(m_serialized.end(), temp.crbegin(), temp.crend());
        }

        return *this;
    }

    template <class T>
    Serializer& AppendLittleEndian(const T& t)
    {
        std::vector<uint8_t> temp(sizeof(T));
        memcpy(&temp[0], &t, sizeof(T));

        if (EndianUtil::IsBigEndian())
        {
            m_serialized.insert(m_serialized.end(), temp.crbegin(), temp.crend());
        }
        else
        {
            m_serialized.insert(m_serialized.end(), temp.cbegin(), temp.cend());
        }

        return *this;
    }

    Serializer& AppendByteVector(const std::vector<uint8_t>& vectorToAppend)
    {
        m_serialized.insert(m_serialized.end(), vectorToAppend.cbegin(), vectorToAppend.cend());
        return *this;
    }

    Serializer& AppendVarStr(const std::string& varString)
    {
        size_t stringLength = varString.length();
        Append<uint64_t>(stringLength);
        m_serialized.insert(m_serialized.end(), varString.cbegin(), varString.cend());
        return *this;
    }

    const std::vector<uint8_t>& vec() const { return m_serialized; }
    const uint8_t* data() const { return m_serialized.data(); }
    size_t size() const { return m_serialized.size(); }

    uint8_t& operator[] (const size_t x) { return m_serialized[x]; }
    const uint8_t& operator[] (const size_t x) const { return m_serialized[x]; }


    // WARNING: This will destroy the contents of m_serialized.
    // TODO: Create a SecureSerializer instead.
    //SecureVector GetSecureBytes()
    //{
    //    SecureVector secureBytes(m_serialized.begin(), m_serialized.end());
    //    cleanse(m_serialized.data(), m_serialized.size());

    //    return secureBytes;
    //}

private:
    std::vector<uint8_t> m_serialized;
};