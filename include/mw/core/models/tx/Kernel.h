#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/crypto/Crypto.h>
#include <mw/core/traits/Committed.h>
#include <mw/core/traits/Hashable.h>
#include <mw/core/traits/Serializable.h>
#include <mw/core/traits/Jsonable.h>
#include <mw/core/models/tx/Features.h>
#include <mw/core/models/crypto/Signature.h>

////////////////////////////////////////
// TRANSACTION KERNEL
////////////////////////////////////////
class Kernel :
    public Traits::ICommitted,
    public Traits::IHashable,
    public Traits::ISerializable,
    public Traits::IJsonable
{
public:
    //
    // Constructors
    //
    Kernel(
        const EKernelFeatures features,
        const uint64_t fee,
        const uint64_t lockHeight,
        Commitment&& excessCommitment,
        Signature&& excessSignature
    )
        : m_features(features),
        m_fee(fee),
        m_lockHeight(lockHeight),
        m_excessCommitment(std::move(excessCommitment)),
        m_excessSignature(std::move(excessSignature))
    {
        Serializer serializer;
        Serialize(serializer);
        m_hash = Crypto::Blake2b(serializer.vec());
    }

    Kernel(const Kernel& kernel) = default;
    Kernel(Kernel&& kernel) noexcept = default;
    Kernel() = default;

    //
    // Destructor
    //
    virtual ~Kernel() = default;

    //
    // Operators
    //
    Kernel& operator=(const Kernel& kernel) = default;
    Kernel& operator=(Kernel&& kernel) noexcept = default;
    bool operator<(const Kernel& kernel) const { return m_hash < kernel.m_hash; }
    bool operator==(const Kernel& kernel) const { return m_hash == kernel.m_hash; }
    bool operator!=(const Kernel& kernel) const { return m_hash != kernel.m_hash; }

    //
    // Getters
    //
    EKernelFeatures GetFeatures() const { return m_features; }
    uint64_t GetFee() const { return m_fee; }
    uint64_t GetLockHeight() const { return m_lockHeight; }
    const Commitment& GetExcessCommitment() const { return m_excessCommitment; }
    const Signature& GetExcessSignature() const { return m_excessSignature; }
    bool IsCoinbase() const { return (m_features & EKernelFeatures::COINBASE_KERNEL) == EKernelFeatures::COINBASE_KERNEL; }
    Hash GetSignatureMessage() const
    {
        Serializer serializer;
        serializer.Append<uint8_t>((uint8_t)GetFeatures());

        if (GetFeatures() != EKernelFeatures::COINBASE_KERNEL)
        {
            serializer.Append<uint64_t>(GetFee());
        }

        if (GetFeatures() == EKernelFeatures::HEIGHT_LOCKED)
        {
            serializer.Append<uint64_t>(GetLockHeight());
        }

        return Crypto::Blake2b(serializer.vec());
    }

    //
    // Serialization/Deserialization
    //
    virtual Serializer& Serialize(Serializer& serializer) const override final
    {
        // TODO: Implement
        return serializer;
    }

    static Kernel Deserialize(Deserializer&)
    {
        // TODO: Implement
        return Kernel();
    }

    virtual json ToJSON() const override final
    {
        // TODO: Implement
        return json();
    }

    static Kernel FromJSON(const json&)
    {
        // TODO: Implement
        return Kernel();
    }

    //
    // Traits
    //
    virtual Hash GetHash() const override final { return m_hash; }
    virtual const Commitment& GetCommitment() const override final { return m_excessCommitment; }

private:
    // Options for a kernel's structure or use
    EKernelFeatures m_features;

    // Fee originally included in the transaction this proof is for.
    uint64_t m_fee;

    // This kernel is not valid earlier than m_lockHeight blocks
    // The max m_lockHeight of all *inputs* to this transaction
    uint64_t m_lockHeight;

    // Remainder of the sum of all transaction commitments. 
    // If the transaction is well formed, amounts components should sum to zero and the excess is hence a valid public key.
    Commitment m_excessCommitment;

    // The signature proving the excess is a valid public key, which signs the transaction fee.
    Signature m_excessSignature;

    mutable Hash m_hash;
};