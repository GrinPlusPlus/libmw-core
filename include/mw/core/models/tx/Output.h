#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/Context.h>
#include <mw/core/models/tx/Features.h>
#include <mw/core/models/crypto/RangeProof.h>
#include <mw/core/crypto/Crypto.h>
#include <mw/core/traits/Committed.h>
#include <mw/core/traits/Hashable.h>
#include <mw/core/traits/Serializable.h>
#include <mw/core/traits/Jsonable.h>

////////////////////////////////////////
// OUTPUT
////////////////////////////////////////
class Output :
    public Traits::ICommitted,
    public Traits::IHashable,
    public Traits::ISerializable,
    public Traits::IJsonable
{
public:
    //
    // Constructors
    //
    Output(const EOutputFeatures features, Commitment&& commitment, const RangeProof::CPtr& pProof)
        : m_features(features), m_commitment(std::move(commitment)), m_pProof(pProof)
    {
        Serializer serializer;
        Serialize(serializer);
        m_hash = Crypto::Blake2b(serializer.vec());
    }
    Output(const Output& Output) = default;
    Output(Output&& Output) noexcept = default;
    Output() = default;

    //
    // Destructor
    //
    virtual ~Output() = default;

    //
    // Operators
    //
    Output& operator=(const Output& Output) = default;
    Output& operator=(Output&& Output) noexcept = default;
    bool operator<(const Output& Output) const noexcept { return m_hash < Output.m_hash; }
    bool operator==(const Output& Output) const noexcept { return m_hash == Output.m_hash; }

    //
    // Getters
    //
    EOutputFeatures GetFeatures() const noexcept { return m_features; }
    virtual const Commitment& GetCommitment() const noexcept override final { return m_commitment; }
    const RangeProof::CPtr& GetRangeProof() const noexcept { return m_pProof; }

    bool IsCoinbase() const noexcept { return (m_features & EOutputFeatures::COINBASE_OUTPUT) == EOutputFeatures::COINBASE_OUTPUT; }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const noexcept override final
    {
        return serializer
            .Append<uint8_t>((uint8_t)m_features)
            .Append(m_commitment)
            .Append(m_pProof);
    }

    static Output Deserialize(const Context::CPtr&, Deserializer& deserializer)
    {
        const EOutputFeatures features = (EOutputFeatures)deserializer.Read<uint8_t>();
        Commitment commitment = Commitment::Deserialize(deserializer);
        RangeProof::CPtr pProof = std::make_shared<const RangeProof>(RangeProof::Deserialize(deserializer));
        return Output(features, std::move(commitment), pProof);
    }

    virtual json ToJSON() const noexcept override final
    {
        return json({
            {"features", OutputFeatures::ToString(m_features)},
            {"commit", m_commitment},
            {"proof", m_pProof}
        });
    }

    static Output FromJSON(const Json& json)
    {
        return Output(
            OutputFeatures::FromString(json.GetRequired<std::string>("features")),
            json.GetRequired<Commitment>("commit"),
            std::make_shared<const RangeProof>(json.GetRequired<RangeProof>("proof"))
        );
    }

    //
    // Traits
    //
    virtual Hash GetHash() const noexcept override final { return m_hash; }

private:
    // Options for an output's structure or use
    EOutputFeatures m_features;

    // The homomorphic commitment representing the output amount
    Commitment m_commitment;

    // A proof that the commitment is in the right range
    RangeProof::CPtr m_pProof;

    mutable Hash m_hash;
};