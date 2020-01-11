#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

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
    Output(const EOutputFeatures features, Commitment&& commitment, RangeProof&& proof)
        : m_features(features), m_commitment(std::move(commitment)), m_proof(std::move(proof))
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
    bool operator<(const Output& Output) const { return m_hash < Output.m_hash; }
    bool operator==(const Output& Output) const { return m_hash == Output.m_hash; }

    //
    // Getters
    //
    EOutputFeatures GetFeatures() const { return m_features; }
    virtual const Commitment& GetCommitment() const override final { return m_commitment; }
    const RangeProof& GetRangeProof() const { return m_proof; }

    bool IsCoinbase() const { return (m_features & EOutputFeatures::COINBASE_OUTPUT) == EOutputFeatures::COINBASE_OUTPUT; }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        serializer.Append<uint8_t>((uint8_t)m_features);
        m_commitment.Serialize(serializer);
        m_proof.Serialize(serializer);
        return serializer;
    }

    static Output Deserialize(ByteBuffer& byteBuffer)
    {
        const EOutputFeatures features = (EOutputFeatures)byteBuffer.ReadU8();
        Commitment commitment = Commitment::Deserialize(byteBuffer);
        RangeProof proof = RangeProof::Deserialize(byteBuffer);
        return Output(features, std::move(commitment), std::move(proof));
    }

    virtual json ToJSON() const override final
    {
        return json({
            {"features", OutputFeatures::ToString(m_features)},
            {"commit", m_commitment},
            {"proof", m_proof}
        });
    }

    static Output FromJSON(const json& json)
    {
        return Output(
            OutputFeatures::FromString(json["features"].get<std::string>()),
            json["commit"].get<Commitment>(),
            json["proof"].get<RangeProof>()
        );
    }

    //
    // Traits
    //
    virtual Hash GetHash() const override final { return m_hash; }

private:
    // Options for an output's structure or use
    EOutputFeatures m_features;

    // The homomorphic commitment representing the output amount
    Commitment m_commitment;

    // A proof that the commitment is in the right range
    RangeProof m_proof;

    mutable Hash m_hash;
};