#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/Context.h>
#include <mw/core/models/block/Header.h>
#include <mw/core/models/tx/TxBody.h>
#include <mw/core/traits/Printable.h>
#include <mw/core/traits/Serializable.h>
#include <mw/core/traits/Hashable.h>

class Block :
    public Traits::IPrintable,
    public Traits::ISerializable,
    public Traits::IHashable
{
public:
    using CPtr = std::shared_ptr<const Block>;

    //
    // Constructors
    //
    Block(const IHeader::CPtr& pHeader, TxBody&& body)
        : m_pHeader(pHeader), m_body(std::move(body)), m_validated(false)
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
    const IHeader::CPtr& GetHeader() const { return m_pHeader; }
    const TxBody& GetTxBody() const { return m_body; }

    const std::vector<Input>& GetInputs() const { return m_body.GetInputs(); }
    const std::vector<Output>& GetOutputs() const { return m_body.GetOutputs(); }
    const std::vector<Kernel>& GetKernels() const { return m_body.GetKernels(); }

    uint64_t GetHeight() const { return m_pHeader->GetHeight(); }
    const Hash& GetPreviousHash() const { return m_pHeader->GetPreviousHash(); }
    const BlindingFactor& GetOffset() const { return m_pHeader->GetOffset(); }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        return serializer.Append(m_pHeader).Append(m_body);
    }

    static Block::CPtr Deserialize(const Context::CPtr& pContext, Deserializer& deserializer)
    {
        IHeader::CPtr pHeader = pContext->GetHeaderFactory().Deserialize(pContext, deserializer);
        TxBody body = TxBody::Deserialize(pContext, deserializer);
        return std::make_shared<const Block>(pHeader, std::move(body));
    }

    //
    // Validates all the elements in a block that can be checked without additional data. 
    // Includes commitment sums, kernels, reward, etc.
    //
    virtual void Validate() const { }

    //
    // Traits
    //
    virtual Hash GetHash() const { return m_pHeader->GetHash(); }
    virtual std::string Format() const override final { return GetHash().ToHex(); }

private:
    IHeader::CPtr m_pHeader;
    TxBody m_body;
    mutable bool m_validated;
};