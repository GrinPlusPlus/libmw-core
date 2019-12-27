#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/models/crypto/BigInteger.h>
#include <mw/traits/Serializable.h>
#include <mw/traits/Jsonable.h>
#include <mw/models/tx/Input.h>
#include <mw/models/tx/Output.h>
#include <mw/models/tx/Kernel.h>

#include <memory>
#include <vector>

////////////////////////////////////////
// TRANSACTION BODY - Container for all inputs, outputs, and kernels in a transaction or block.
////////////////////////////////////////
class TxBody :
    public Traits::ISerializable,
    public Traits::IJsonable
{
public:
    using CPtr = std::shared_ptr<const TxBody>;

    //
    // Constructors
    //
    TxBody(std::vector<Input>&& inputs, std::vector<Output>&& outputs, std::vector<Kernel>&& kernels)
        : m_inputs(std::move(inputs)), m_outputs(std::move(outputs)), m_kernels(std::move(kernels))
    {

    }
    TxBody(const TxBody& other) = default;
    TxBody(TxBody&& other) noexcept = default;
    TxBody() = default;

    //
    // Destructor
    //
    virtual ~TxBody() = default;

    //
    // Operators
    //
    TxBody& operator=(const TxBody& other) = default;
    TxBody& operator=(TxBody&& other) noexcept = default;

    //
    // Getters
    //
    const std::vector<Input>& GetInputs() const { return m_inputs; }
    const std::vector<Output>& GetOutputs() const { return m_outputs; }
    const std::vector<Kernel>& GetKernels() const { return m_kernels; }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        serializer
            .Append<uint64_t>(m_inputs.size())
            .Append<uint64_t>(m_outputs.size())
            .Append<uint64_t>(m_kernels.size());

        std::for_each(m_inputs.cbegin(), m_inputs.cend(), [&serializer](const auto& input) { input.Serialize(serializer); });
        std::for_each(m_outputs.cbegin(), m_outputs.cend(), [&serializer](const auto& output) { output.Serialize(serializer); });
        std::for_each(m_kernels.cbegin(), m_kernels.cend(), [&serializer](const auto& kernel) { kernel.Serialize(serializer); });

        return serializer;
    }

    static TxBody Deserialize(ByteBuffer& byteBuffer)
    {
        // TODO: Implement
    }

    virtual json ToJSON() const override final
    {
        return json({
            {"inputs", m_inputs},
            {"outputs", m_outputs},
            {"kernels", m_kernels}
        });
    }

    static TxBody FromJSON(const json& json)
    {
        // TODO: Implement
    }

private:
    // List of inputs spent by the transaction.
    std::vector<Input> m_inputs;

    // List of outputs the transaction produces.
    std::vector<Output> m_outputs;

    // List of kernels that make up this transaction (usually a single kernel).
    std::vector<Kernel> m_kernels;
};