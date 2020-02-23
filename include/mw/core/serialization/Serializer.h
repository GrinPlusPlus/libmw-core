#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/common/Secure.h>
#include <mw/core/util/EndianUtil.h>
#include <mw/core/traits/Serializable.h>

#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <algorithm>

class Serializer
{
public:
    Serializer() = default;
    Serializer(const size_t expectedSize) { m_serialized.reserve(expectedSize); }
    ~Serializer() { SecureMem::cleanse(m_serialized.data(), m_serialized.size()); }

    template <class T, typename SFINAE = typename std::enable_if_t<std::is_integral_v<T>>>
    Serializer& Append(const T& t)
    {
        std::vector<uint8_t> temp(sizeof(T));
        memcpy(temp.data(), &t, sizeof(T));

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

    template <class T, typename SFINAE = typename std::enable_if_t<std::is_integral_v<T>>>
    Serializer& AppendLE(const T& t)
    {
        std::vector<uint8_t> temp(sizeof(T));
        memcpy(temp.data(), &t, sizeof(T));

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

    Serializer& Append(const std::vector<uint8_t>& vectorToAppend)
    {
        m_serialized.insert(m_serialized.end(), vectorToAppend.cbegin(), vectorToAppend.cend());
        return *this;
    }

    template <size_t T>
    Serializer& Append(const std::array<uint8_t, T>& arr)
    {
        m_serialized.insert(m_serialized.end(), arr.cbegin(), arr.cend());
        return *this;
    }

    // TODO: Should we care about unicode, where chars are larger than 1 byte?
    Serializer& Append(const std::string& varString)
    {
        Append<uint64_t>(varString.length());
        m_serialized.insert(m_serialized.end(), varString.cbegin(), varString.cend());
        return *this;
    }

    Serializer& Append(const char* str)
    {
        return Append(std::string(str));
    }

    Serializer& Append(const Traits::ISerializable& serializable)
    {
        return serializable.Serialize(*this);
    }

    Serializer& Append(const std::shared_ptr<const Traits::ISerializable>& pSerializable)
    {
        return pSerializable->Serialize(*this);
    }

    const std::vector<uint8_t>& vec() const { return m_serialized; }
    const uint8_t* data() const { return m_serialized.data(); }
    size_t size() const { return m_serialized.size(); }

    uint8_t& operator[] (const size_t x) { return m_serialized[x]; }
    const uint8_t& operator[] (const size_t x) const { return m_serialized[x]; }

private:
    std::vector<uint8_t> m_serialized;
};