#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/Context.h>
#include <mw/core/crypto/Crypto.h>
#include <mw/core/traits/Committed.h>
#include <mw/core/traits/Hashable.h>
#include <mw/core/traits/Serializable.h>
#include <mw/core/traits/Printable.h>
#include <mw/core/traits/Jsonable.h>
#include <mw/core/models/crypto/Signature.h>
#include <tl/optional.hpp>

////////////////////////////////////////
// TRANSACTION KERNEL
////////////////////////////////////////
class IKernel :
    public Traits::ICommitted,
    public Traits::IHashable,
    public Traits::ISerializable,
    public Traits::IJsonable
{
public:
    using CPtr = std::shared_ptr<const IKernel>;

    //
    // Constructors
    //
    IKernel(
        const uint8_t features,
        const uint64_t fee,
        const uint64_t lockHeight,
        Commitment&& excess,
        Signature&& signature
    )
        : m_features(features),
        m_fee(fee),
        m_lockHeight(lockHeight),
        m_excess(std::move(excess)),
        m_signature(std::move(signature)),
        m_hash(tl::nullopt) { }

    IKernel(const IKernel& kernel) = default;
    IKernel(IKernel&& kernel) noexcept = default;
    IKernel() = default;

    //
    // Destructor
    //
    virtual ~IKernel() = default;

    //
    // Operators
    //
    IKernel& operator=(const IKernel& kernel) = default;
    IKernel& operator=(IKernel&& kernel) noexcept = default;
    bool operator<(const IKernel& kernel) const noexcept { return GetHash() < kernel.GetHash(); }
    bool operator==(const IKernel& kernel) const noexcept { return GetHash() == kernel.GetHash(); }
    bool operator!=(const IKernel& kernel) const noexcept { return GetHash() != kernel.GetHash(); }

    //
    // Getters
    //
    const uint8_t GetFeatures() const noexcept { return m_features; }
    uint64_t GetFee() const noexcept { return m_fee; }
    uint64_t GetLockHeight() const noexcept { return m_lockHeight; }
    const Commitment& GetExcess() const noexcept { return m_excess; }
    const Signature& GetSignature() const noexcept { return m_signature; }
    virtual Hash GetSignatureMessage() const = 0;

    virtual void Validate() = 0;

    //
    // Serialization/Deserialization
    //
    static IKernel::CPtr Deserialize(const Context::CPtr& pCtx, Deserializer& deserializer)
    {
        return pCtx->GetKernelFactory().Deserialize(deserializer);
    }

    static IKernel::CPtr FromJSON(const Context::CPtr& pCtx, const Json& json)
    {
        return pCtx->GetKernelFactory().FromJSON(json);
    }

    //
    // Traits
    //
    Hash GetHash() const noexcept final
    {
        if (!m_hash.has_value())
        {
            m_hash = tl::make_optional(Crypto::Blake2b(Serialized()));
        }

        return m_hash.value(); 
    }
    const Commitment& GetCommitment() const noexcept final { return m_excess; }

protected:
    // Options for a kernel's structure or use
    uint8_t m_features;

    // Fee originally included in the transaction this proof is for.
    uint64_t m_fee;

    // This kernel is not valid earlier than m_lockHeight blocks
    // The max m_lockHeight of all *inputs* to this transaction
    uint64_t m_lockHeight;

    // Remainder of the sum of all transaction commitments. 
    // If the transaction is well formed, amounts components should sum to zero and the excess is hence a valid public key.
    Commitment m_excess;

    // The signature proving the excess is a valid public key, which signs the transaction fee.
    Signature m_signature;

    mutable tl::optional<Hash> m_hash;
};