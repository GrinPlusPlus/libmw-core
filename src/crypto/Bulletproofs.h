#pragma once

#include "Context.h"
#include "BulletproofsCache.h"

#include<mw/core/models/crypto/Commitment.h>
#include<mw/core/models/crypto/RangeProof.h>
#include<mw/core/models/crypto/BlindingFactor.h>
#include<mw/core/models/crypto/ProofMessage.h>
#include<mw/core/models/crypto/RewoundProof.h>

class Bulletproofs
{
public:
    Bulletproofs(Locked<Context>& context) : m_context(context) { }
    ~Bulletproofs() = default;

    bool VerifyBulletproofs(
        const std::vector<std::pair<Commitment, RangeProof>>& rangeProofs
    ) const;

    RangeProof GenerateRangeProof(
        const uint64_t amount,
        const SecretKey& key,
        const SecretKey& privateNonce,
        const SecretKey& rewindNonce,
        const ProofMessage& proofMessage
    );

    std::unique_ptr<RewoundProof> RewindProof(
        const Commitment& commitment,
        const RangeProof& rangeProof,
        const SecretKey& nonce
    ) const;

private:
    Locked<Context> m_context;

    secp256k1_bulletproof_generators* m_pGenerators;
    mutable BulletProofsCache m_cache;
};