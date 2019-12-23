#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/models/BlockHeader.h>
#include <mw/models/TransactionBody.h>
#include <mw/traits/Printable.h>
#include <mw/traits/Serializable.h>

class Block : public Traits::IPrintable, public Traits::ISerializable
{
public:
    //
    // Constructors
    //
    Block(IBlockHeader::CPtr pBlockHeader, TransactionBody&& transactionBody)
        : m_pBlockHeader(pBlockHeader), m_transactionBody(std::move(transactionBody)), m_validated(false)
    {

    }
    Block(const Block& other) = default;
    Block(Block&& other) noexcept = default;
    Block() = default;

    //
    // Destructor
    //
    virtual ~Block() = default;

    //
    // Operators
    //
    Block& operator=(const Block& other) = default;
    Block& operator=(Block&& other) noexcept = default;

    //
    // Getters
    //
    const IBlockHeader::CPtr& GetBlockHeader() const { return m_pBlockHeader; }
    const TransactionBody& GetTransactionBody() const { return m_transactionBody; }

    const std::vector<TransactionInput>& GetInputs() const { return m_transactionBody.GetInputs(); }
    const std::vector<TransactionOutput>& GetOutputs() const { return m_transactionBody.GetOutputs(); }
    const std::vector<TransactionKernel>& GetKernels() const { return m_transactionBody.GetKernels(); }

    uint64_t GetHeight() const { return m_pBlockHeader->GetHeight(); }
    const Hash& GetPreviousHash() const { return m_pBlockHeader->GetPreviousBlockHash(); }
    uint64_t GetTotalDifficulty() const { return m_pBlockHeader->GetTotalDifficulty(); }
    const BlindingFactor& GetTotalKernelOffset() const { return m_pBlockHeader->GetTotalKernelOffset(); }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        m_pBlockHeader->Serialize(serializer);
        m_transactionBody.Serialize(serializer);
        return serializer;
    }
    static Block Deserialize(ByteBuffer& byteBuffer); // TODO: How do we deserialize header?

    //
    // Hashing
    //
    const Hash& GetHash() const { return m_pBlockHeader->GetHash(); }

    //
    // Validates all the elements in a block that can be checked without additional data. 
    // Includes commitment sums, kernels, reward, etc.
    //
    void Validate() const;

    //
    // Traits
    //
    virtual std::string Format() const override final { return GetHash().ToHex(); }

private:
    IBlockHeader::CPtr m_pBlockHeader;
    TransactionBody m_transactionBody;
    mutable bool m_validated;
};