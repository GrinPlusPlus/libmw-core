#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/models/crypto/BlindingFactor.h>
#include <mw/core/models/crypto/Hash.h>
#include <mw/core/traits/Hashable.h>
#include <mw/core/traits/Printable.h>
#include <mw/core/serialization/Serializer.h>
#include <mw/core/crypto/Crypto.h>

#include <cstdint>
#include <memory>

// Forward Declarations
class NodeContext;

class IHeader :
    public Traits::IPrintable,
    public Traits::ISerializable,
    public Traits::IHashable
{
public:
    using CPtr = std::shared_ptr<const IHeader>;

    //
    // Constructors
    //
    IHeader(
        const uint16_t version,
        const uint64_t height,
        Hash&& previousHash,
        Hash&& previousRoot,
        Hash&& outputRoot,
        Hash&& rangeProofRoot,
        Hash&& kernelRoot,
        BlindingFactor&& offset,
        const uint64_t outputMMRSize,
        const uint64_t kernelMMRSize
    )
        : m_version(version),
        m_height(height),
        m_previousHash(std::move(previousHash)),
        m_previousRoot(std::move(previousRoot)),
        m_outputRoot(std::move(outputRoot)),
        m_rangeProofRoot(std::move(rangeProofRoot)),
        m_kernelRoot(std::move(kernelRoot)),
        m_offset(std::move(offset)),
        m_outputMMRSize(outputMMRSize),
        m_kernelMMRSize(kernelMMRSize)
    {
        Serializer serializer;
        m_hash = Crypto::Blake2b(Serialize(serializer).vec());
    }

    //
    // Destructor
    //
    virtual ~IHeader() = default;

    //
    // Operators
    //
    bool operator!=(const IHeader& rhs) const { return this->GetHash() != rhs.GetHash(); }

    //
    // Getters
    //
    uint16_t GetVersion() const { return m_version; }
    uint64_t GetHeight() const { return m_height; }
    const Hash& GetPreviousHash() const { return m_previousHash; }
    const Hash& GetPreviousRoot() const { return m_previousRoot; }
    const Hash& GetOutputRoot() const { return m_outputRoot; }
    const Hash& GetRangeProofRoot() const { return m_rangeProofRoot; }
    const Hash& GetKernelRoot() const { return m_kernelRoot; }
    const BlindingFactor& GetOffset() const { return m_offset; }
    uint64_t GetOutputMMRSize() const { return m_outputMMRSize; }
    uint64_t GetKernelMMRSize() const { return m_kernelMMRSize; }

    //
    // Validation
    //
    virtual void Validate(const NodeContext& context) const = 0;

    //
    // Traits
    //
    virtual Hash GetHash() const override final { return m_hash; }
    virtual std::string Format() const { return GetHash().ToHex(); }

protected:
    mutable Hash m_hash;
    uint16_t m_version;
    uint64_t m_height;
    Hash m_previousHash;
    Hash m_previousRoot;
    Hash m_outputRoot;
    Hash m_rangeProofRoot;
    Hash m_kernelRoot;
    BlindingFactor m_offset;
    uint64_t m_outputMMRSize;
    uint64_t m_kernelMMRSize;
};