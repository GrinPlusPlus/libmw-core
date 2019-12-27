#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/traits/Printable.h>
#include <mw/traits/Serializable.h>
#include <mw/traits/Jsonable.h>
#include <mw/util/HexUtil.h>

#include <cassert>
#include <cstdint>
#include <vector>

class RangeProof :
    public Traits::IPrintable,
    public Traits::ISerializable,
    public Traits::IJsonable
{
public:
    static constexpr size_t const& MAX_SIZE = 675;

    //
    // Constructors
    //
    RangeProof() = default;
    RangeProof(std::vector<uint8_t>&& bytes) : m_bytes(std::move(bytes)) { assert(m_bytes.size() <= MAX_SIZE); }
    RangeProof(const RangeProof& other) = default;
    RangeProof(RangeProof&& other) noexcept = default;

    //
    // Destructor
    //
    virtual ~RangeProof() = default;

    //
    // Operators
    //
    RangeProof& operator=(const RangeProof& other) = default;
    RangeProof& operator=(RangeProof&& other) noexcept = default;

    //
    // Getters
    //
    const std::vector<uint8_t>& vec() const { return m_bytes; }
    const uint8_t* data() const { return m_bytes.data(); }
    size_t size() const { return m_bytes.size(); }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        return serializer
            .Append<uint64_t>(m_bytes.size())
            .AppendByteVector(m_bytes);
    }

    static RangeProof Deserialize(ByteBuffer& byteBuffer)
    {
        const uint64_t proofSize = byteBuffer.ReadU64();
        if (proofSize > MAX_SIZE)
        {
            throw DeserializationEx("RangeProof is larger than MAX_SIZE");
        }

        return RangeProof(byteBuffer.ReadVector(proofSize));
    }

    virtual json ToJSON() const override final
    {
        return json(ToHex());
    }

    static RangeProof FromJSON(const json& json)
    {
        return RangeProof::FromHex(json.get<std::string>());
    }

    std::string ToHex() const { return HexUtil::ToHex(m_bytes); }
    static RangeProof FromHex(const std::string& hex) { return RangeProof(HexUtil::FromHex(hex)); }

    //
    // Traits
    //
    virtual std::string Format() const override final { return HexUtil::ToHex(m_bytes); }

private:
    // The proof itself, at most 675 bytes long.
    std::vector<uint8_t> m_bytes;
};
