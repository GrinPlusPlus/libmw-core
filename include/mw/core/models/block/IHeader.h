#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/Context.h>
#include <mw/core/models/crypto/BlindingFactor.h>
#include <mw/core/models/crypto/Hash.h>
#include <mw/core/traits/Hashable.h>
#include <mw/core/traits/Serializable.h>
#include <mw/core/traits/Printable.h>
#include <mw/core/traits/Jsonable.h>
#include <mw/core/serialization/Serializer.h>
#include <mw/core/crypto/Crypto.h>

#include <cstdint>
#include <memory>

class IHeader :
    public Traits::IPrintable,
    public Traits::ISerializable,
    public Traits::IHashable,
    public Traits::IJsonable
{
public:
    using CPtr = std::shared_ptr<const IHeader>;

    //
    // Constructors
    //
    IHeader(
        const uint64_t height,
        Hash&& previousHash,
        Hash&& outputRoot,
        Hash&& rangeProofRoot,
        Hash&& kernelRoot,
        BlindingFactor&& offset,
        const uint64_t outputMMRSize,
        const uint64_t kernelMMRSize
    )
        : m_height(height),
        m_previousHash(std::move(previousHash)),
        m_outputRoot(std::move(outputRoot)),
        m_rangeProofRoot(std::move(rangeProofRoot)),
        m_kernelRoot(std::move(kernelRoot)),
        m_offset(std::move(offset)),
        m_outputMMRSize(outputMMRSize),
        m_kernelMMRSize(kernelMMRSize)
    {

    }

    //
    // Destructor
    //
    virtual ~IHeader() = default;

    //
    // Operators
    //
    bool operator!=(const IHeader& rhs) const noexcept { return this->GetHash() != rhs.GetHash(); }

    //
    // Getters
    //
    uint64_t GetHeight() const noexcept { return m_height; }
    const Hash& GetPreviousHash() const noexcept { return m_previousHash; }
    const Hash& GetOutputRoot() const noexcept { return m_outputRoot; }
    const Hash& GetRangeProofRoot() const noexcept { return m_rangeProofRoot; }
    const Hash& GetKernelRoot() const noexcept { return m_kernelRoot; }
    const BlindingFactor& GetOffset() const noexcept { return m_offset; }
    uint64_t GetOutputMMRSize() const noexcept { return m_outputMMRSize; }
    uint64_t GetKernelMMRSize() const noexcept { return m_kernelMMRSize; }

    //
    // Validation
    //
    virtual void Validate(const Context& context) const = 0;

    //
    // Traits
    //
    Hash GetHash() const noexcept final
    {
        if (!m_hash.has_value())
        {
            m_hash = tl::make_optional(Crypto::Blake2b(Serialized()));
        }

        return m_hash.value();
    }
    virtual std::string Format() const { return GetHash().ToHex(); }

    //
    // Serialization/Deserialization
    //
    static IHeader::CPtr Deserialize(const Context::CPtr& pContext, Deserializer& deserializer)
    {
        return pContext->GetHeaderFactory().Deserialize(deserializer);
    }

    static IHeader::CPtr FromJSON(const Context::CPtr& pContext, const Json& json)
    {
        return pContext->GetHeaderFactory().FromJSON(json);
    }

protected:
    mutable tl::optional<Hash> m_hash;
    uint64_t m_height;
    Hash m_previousHash;
    Hash m_outputRoot;
    Hash m_rangeProofRoot;
    Hash m_kernelRoot;
    BlindingFactor m_offset;
    uint64_t m_outputMMRSize;
    uint64_t m_kernelMMRSize;
};