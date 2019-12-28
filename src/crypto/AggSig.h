#pragma once

#include "Context.h"

#include <mw/core/models/crypto/Commitment.h>
#include <mw/core/models/crypto/SecretKey.h>
#include <mw/core/models/crypto/Signature.h>
#include <mw/core/models/crypto/PublicKey.h>
#include <mw/core/models/crypto/Hash.h>

#include <shared_mutex>

class AggSig
{
public:
    AggSig(Locked<Context>& context) : m_context(context) { }
    ~AggSig() = default;

    SecretKey GenerateSecureNonce() const;

    //
    // Signs the message hash with the given key.
    //
    CompactSignature::UPtr SignMessage(
        const SecretKey& secretKey,
        const PublicKey& publicKey,
        const Hash& message
    );

    //
    // Verifies the signature is valid for the message hash and public key.
    //
    bool VerifyMessageSignature(
        const CompactSignature& signature,
        const PublicKey& publicKey,
        const Hash& message
    ) const;

    CompactSignature::UPtr CalculatePartialSignature(
        const SecretKey& secretKey,
        const SecretKey& secretNonce,
        const PublicKey& sumPubKeys,
        const PublicKey& sumPubNonces,
        const Hash& message
    );

    bool VerifyPartialSignature(
        const CompactSignature& partialSignature,
        const PublicKey& publicKey,
        const PublicKey& sumPubKeys,
        const PublicKey& sumPubNonces,
        const Hash& message
    ) const;

    Signature::UPtr AggregateSignatures(
        const std::vector<CompactSignature>& signatures,
        const PublicKey& sumPubNonces
    ) const;

    bool VerifyAggregateSignatures(
        const std::vector<const Signature*>& signatures,
        const std::vector<const Commitment*>& publicKeys,
        const std::vector<const Hash*>& messages
    ) const;

    bool VerifyAggregateSignature(
        const Signature& signature,
        const PublicKey& sumPubKeys,
        const Hash& message
    ) const;

private:
    std::vector<secp256k1_ecdsa_signature> ParseCompactSignatures(
        const secp256k1_context* pContext,
        const std::vector<CompactSignature>& signatures
    ) const;

    Locked<Context> m_context;
};