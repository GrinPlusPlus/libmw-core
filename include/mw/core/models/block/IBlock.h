#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/Context.h>
#include <mw/core/models/Block/IHeader.h>
#include <mw/core/models/tx/TxBody.h>
#include <mw/core/traits/Hashable.h>
#include <mw/core/traits/Serializable.h>
#include <mw/core/traits/Printable.h>
#include <mw/core/traits/Jsonable.h>

class IBlock :
    public Traits::IPrintable,
    public Traits::ISerializable,
    public Traits::IHashable,
    public Traits::IJsonable
{
public:
    using CPtr = std::shared_ptr<const IBlock>;

    //
    // Constructors
    //
    IBlock(const IHeader::CPtr& pHeader, TxBody&& body)
        : m_pHeader(pHeader), m_body(std::move(body)), m_validated(false) { }
    IBlock(const IBlock& other) = delete;
    IBlock(IBlock&& other) noexcept = default;
    IBlock() = default;

    //
    // Destructor
    //
    virtual ~IBlock() = default;

    //
    // Operators
    //
    IBlock& operator=(const IBlock& other) = delete;
    IBlock& operator=(IBlock&& other) noexcept = default;

    //
    // Getters
    //
    const IHeader::CPtr& GetHeader() const noexcept { return m_pHeader; }
    const TxBody& GetTxBody() const noexcept { return m_body; }

    const std::vector<Input>& GetInputs() const noexcept { return m_body.GetInputs(); }
    const std::vector<Output>& GetOutputs() const noexcept { return m_body.GetOutputs(); }
    const std::vector<IKernel::CPtr>& GetKernels() const noexcept { return m_body.GetKernels(); }

    uint64_t GetHeight() const noexcept { return m_pHeader->GetHeight(); }
    const Hash& GetPreviousHash() const noexcept { return m_pHeader->GetPreviousHash(); }
    const BlindingFactor& GetOffset() const noexcept { return m_pHeader->GetOffset(); }

    //
    // Serialization/Deserialization
    //
    Serializer& Serialize(Serializer& serializer) const noexcept final
    {
        return serializer.Append(m_pHeader).Append(m_body);
    }

    static IBlock::CPtr Deserialize(const Context::CPtr& pContext, Deserializer& deserializer)
    {
        return pContext->GetBlockFactory().Deserialize(deserializer);
    }

    json ToJSON() const noexcept final
    {
        return json({
            { "header", m_pHeader },
            { "body", m_body }
        });
    }

    static IBlock::CPtr FromJSON(const Context::CPtr& pContext, const Json& json)
    {
        return pContext->GetBlockFactory().FromJSON(json);
    }

    //
    // Validates all the elements in a block that can be checked without additional data. 
    // Includes commitment sums, kernels, reward, etc.
    //
    virtual void Validate(const Context::CPtr& pContext) const = 0;

    //
    // Traits
    //
    virtual Hash GetHash() const noexcept { return m_pHeader->GetHash(); }
    std::string Format() const final { return GetHash().ToHex(); }

protected:
    IHeader::CPtr m_pHeader;
    TxBody m_body;
    mutable bool m_validated;
};