#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include<mw/core/models/tx/Features.h>
#include<mw/core/crypto/Crypto.h>
#include<mw/core/traits/Committed.h>
#include<mw/core/traits/Hashable.h>
#include<mw/core/traits/Serializable.h>
#include<mw/core/traits/Jsonable.h>

////////////////////////////////////////
// INPUT
////////////////////////////////////////
class Input :
    public Traits::ICommitted,
    public Traits::IHashable,
    public Traits::ISerializable,
    public Traits::IJsonable
{
public:
    //
    // Constructors
    //
    Input(const EOutputFeatures features, Commitment&& commitment)
        : m_features(features), m_commitment(std::move(commitment))
    {
        Serializer serializer;
        Serialize(serializer);
        m_hash = Crypto::Blake2b(serializer.vec());
    }
    Input(const Input& input) = default;
    Input(Input&& input) noexcept = default;
    Input() = default;

    //
    // Destructor
    //
    virtual ~Input() = default;

    //
    // Operators
    //
    Input& operator=(const Input& input) = default;
    Input& operator=(Input&& input) noexcept = default;
    bool operator<(const Input& input) const { return GetHash() < input.GetHash(); }
    bool operator==(const Input& input) const { return GetHash() == input.GetHash(); }

    //
    // Getters
    //
    EOutputFeatures GetFeatures() const { return m_features; }
    virtual const Commitment& GetCommitment() const override final { return m_commitment; }

    bool IsCoinbase() const { return (m_features & EOutputFeatures::COINBASE_OUTPUT) == EOutputFeatures::COINBASE_OUTPUT; }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        serializer.Append<uint8_t>((uint8_t)m_features);
        m_commitment.Serialize(serializer);
        return serializer;
    }

    static Input Deserialize(ByteBuffer& byteBuffer)
    {
        const EOutputFeatures features = (EOutputFeatures)byteBuffer.ReadU8();
        Commitment commitment = Commitment::Deserialize(byteBuffer);
        return Input(features, std::move(commitment));
    }

    virtual json ToJSON() const override final
    {
        return json({
            {"features", OutputFeatures::ToString(m_features)},
            {"commit", m_commitment}
        });
    }

    static Input FromJSON(const json& json)
    {
        return Input(
            OutputFeatures::FromString(json["features"].get<std::string>()),
            json["commit"].get<Commitment>()
        );
    }

    //
    // Traits
    //
    virtual Hash GetHash() const override final { return m_hash; }

private:
    // The features of the output being spent. 
    // We will check maturity for coinbase output.
    EOutputFeatures m_features;

    // The commit referencing the output being spent.
    Commitment m_commitment;

    mutable Hash m_hash;
};