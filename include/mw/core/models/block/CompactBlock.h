#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/models/block/Header.h>
#include <mw/core/models/block/ShortId.h>
#include <mw/core/models/tx/Output.h>
#include <mw/core/models/tx/Kernel.h>
#include <mw/core/traits/Printable.h>
#include <mw/core/traits/Serializable.h>
#include <mw/core/traits/Hashable.h>

class CompactBlock :
    public Traits::IPrintable,
    public Traits::ISerializable,
    public Traits::IHashable
{
public:
    //
    // Constructors
    //
    CompactBlock(
        IHeader::CPtr pHeader,
        const uint64_t nonce,
        std::vector<Output>&& fullOutputs,
        std::vector<Kernel>&& fullKernels,
        std::vector<ShortId>&& shortIds
    )
        : m_pHeader(pHeader),
        m_nonce(nonce),
        m_outputs(std::move(fullOutputs)),
        m_kernels(std::move(fullKernels)),
        m_shortIds(std::move(shortIds))
    {

    }
    CompactBlock(const CompactBlock& other) = default;
    CompactBlock(CompactBlock&& other) noexcept = default;
    CompactBlock() = default;

    //
    // Destructor
    //
    virtual ~CompactBlock() = default;

    //
    // Operators
    //
    CompactBlock& operator=(const CompactBlock& other) = default;
    CompactBlock& operator=(CompactBlock&& other) noexcept = default;

    //
    // Getters
    //
    const IHeader::CPtr& GetHeader() const { return m_pHeader; }
    uint64_t GetNonce() const { return m_nonce; }

    const std::vector<Output>& GetOutputs() const { return m_outputs; }
    const std::vector<Kernel>& GetKernels() const { return m_kernels; }
    const std::vector<ShortId>& GetShortIds() const { return m_shortIds; }

    const Hash& GetPreviousHash() const { return m_pHeader->GetPreviousHash(); }
    uint64_t GetHeight() const { return m_pHeader->GetHeight(); }

    //
    // Serialization/Deserialization
    //
    Serializer& Serialize(Serializer& serializer) const
    {
        m_pHeader->Serialize(serializer);
        serializer
            .Append<uint64_t>(m_nonce)
            .Append<uint64_t>(m_outputs.size())
            .Append<uint64_t>(m_kernels.size())
            .Append<uint64_t>(m_shortIds.size());

        std::for_each(m_outputs.cbegin(), m_outputs.cend(), [&serializer](const auto& output) { output.Serialize(serializer); });
        std::for_each(m_kernels.cbegin(), m_kernels.cend(), [&serializer](const auto& kernel) { kernel.Serialize(serializer); });
        std::for_each(m_shortIds.cbegin(), m_shortIds.cend(), [&serializer](const auto& shortId) { shortId.Serialize(serializer); });

        return serializer;
    }

    static CompactBlock Deserialize(const Context::CPtr& pContext, Deserializer& deserializer)
    {
        IHeader::CPtr pHeader = pContext->GetHeaderFactory().Deserialize(pContext, deserializer);
        const uint64_t nonce = deserializer.ReadU64();
        const uint64_t numOutputs = deserializer.ReadU64();
        const uint64_t numKernels = deserializer.ReadU64();
        const uint64_t numShortIds = deserializer.ReadU64();

        // Deserialize outputs
        std::vector<Output> outputs;
        outputs.reserve(numOutputs);
        for (uint64_t i = 0; i < numOutputs; i++)
        {
            outputs.emplace_back(Output::Deserialize(pContext, deserializer));
        }

        // Deserialize kernels
        std::vector<Kernel> kernels;
        kernels.reserve(numKernels);
        for (uint64_t i = 0; i < numKernels; i++)
        {
            kernels.emplace_back(Kernel::Deserialize(pContext, deserializer));
        }

        // Deserialize outputs
        std::vector<ShortId> shortIds;
        shortIds.reserve(numShortIds);
        for (uint64_t i = 0; i < numShortIds; i++)
        {
            shortIds.emplace_back(ShortId::Deserialize(pContext, deserializer));
        }

        return CompactBlock(pHeader, nonce, std::move(outputs), std::move(kernels), std::move(shortIds));
    }

    //
    // Traits
    //
    virtual std::string Format() const override final { return m_pHeader->Format(); }
    virtual Hash GetHash() const override final { return m_pHeader->GetHash(); }

private:
    IHeader::CPtr m_pHeader;
    uint64_t m_nonce;
    std::vector<Output> m_outputs;
    std::vector<Kernel> m_kernels;
    std::vector<ShortId> m_shortIds;
};