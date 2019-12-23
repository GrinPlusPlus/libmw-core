#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/models/BlockHeader.h>
#include <mw/models/ShortId.h>
#include <mw/models/TransactionOutput.h>
#include <mw/models/TransactionKernel.h>
#include <mw/traits/Printable.h>
#include <mw/traits/Serializable.h>
#include <mw/traits/Hashable.h>

class CompactBlock : public Traits::IPrintable, public Traits::ISerializable, public Traits::IHashable
{
public:
    //
    // Constructors
    //
    CompactBlock(
        IBlockHeader::CPtr pBlockHeader,
        const uint64_t nonce,
        std::vector<TransactionOutput>&& fullOutputs,
        std::vector<TransactionKernel>&& fullKernels,
        std::vector<ShortId>&& shortIds
    )
        : m_pBlockHeader(pBlockHeader),
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
    const IBlockHeader::CPtr& GetBlockHeader() const { return m_pBlockHeader; }
    uint64_t GetNonce() const { return m_nonce; }

    const std::vector<TransactionOutput>& GetOutputs() const { return m_outputs; }
    const std::vector<TransactionKernel>& GetKernels() const { return m_kernels; }
    const std::vector<ShortId>& GetShortIds() const { return m_shortIds; }

    const Hash& GetPreviousHash() const { return m_pBlockHeader->GetPreviousBlockHash(); }
    uint64_t GetHeight() const { return m_pBlockHeader->GetHeight(); }
    uint64_t GetTotalDifficulty() const { return m_pBlockHeader->GetTotalDifficulty(); }

    //
    // Serialization/Deserialization
    //
    Serializer& Serialize(Serializer& serializer) const
    {
        m_pBlockHeader->Serialize(serializer);
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

    static CompactBlock Deserialize(ByteBuffer& byteBuffer);

    //
    // Traits
    //
    virtual std::string Format() const override final { return m_pBlockHeader->Format(); }
    virtual Hash GetHash() const override final { return m_pBlockHeader->GetHash(); }

private:
    IBlockHeader::CPtr m_pBlockHeader;
    uint64_t m_nonce;
    std::vector<TransactionOutput> m_outputs;
    std::vector<TransactionKernel> m_kernels;
    std::vector<ShortId> m_shortIds;
};