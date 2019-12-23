#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/traits/Printable.h>
#include <mw/traits/Serializable.h>
#include <mw/util/HexUtil.h>

#include <cassert>
#include <cstdint>
#include <vector>

class RangeProof : public Traits::IPrintable, public Traits::ISerializable
{
public:
    using MAX_SIZE = 675;

    //
    // Constructors
    //
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

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        return serializer
            .Append<uint64_t>(m_bytes.size())
            .AppendByteVector(m_bytes);]
    }

    static RangeProof Deserrialize(ByteBuffer& byteBuffer)
    {
        const uint64_t proofSize = byteBuffer.ReadU64();
        if (proofSize > MAX_SIZE)
        {
            throw DESERIALIZATION_EXCEPTION();
        }

        return RangeProof(byteBuffer.ReadVector(proofSize));
    }

    //
    // Traits
    //
    virtual std::string Format() const override final { return HexUtil::ConvertToHex(m_bytes); }

private:
    // The proof itself, at most 675 bytes long.
    std::vector<uint8_t> m_bytes;
};
