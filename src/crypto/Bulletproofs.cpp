#include "Bulletproofs.h"
#include "CommitmentUtil.h"

#include<mw/core/crypto/Random.h>
#include<mw/core/exceptions/CryptoException.h>

const uint64_t MAX_WIDTH = 1 << 20;
const size_t SCRATCH_SPACE_SIZE = 256 * MAX_WIDTH;
const size_t MAX_GENERATORS = 256;

bool Bulletproofs::VerifyBulletproofs(const std::vector<std::pair<Commitment, RangeProof>>& rangeProofs) const
{
    const size_t numBits = 64;
    const size_t proofLength = rangeProofs.front().second.size();

    std::vector<Commitment> commitments;
    commitments.reserve(rangeProofs.size());

    std::vector<const unsigned char*> bulletproofPointers;
    bulletproofPointers.reserve(rangeProofs.size());
    for (const std::pair<Commitment, RangeProof>& rangeProof : rangeProofs)
    {
        if (!m_cache.WasAlreadyVerified(rangeProof.first))
        {
            commitments.push_back(rangeProof.first);
            bulletproofPointers.emplace_back(rangeProof.second.data());
        }
    }

    if (commitments.empty())
    {
        return true;
    }

    // array of generator multiplied by value in pedersen commitments (cannot be NULL)
    std::vector<secp256k1_generator> valueGenerators;
    for (size_t i = 0; i < commitments.size(); i++)
    {
        valueGenerators.push_back(secp256k1_generator_const_h);
    }

    std::vector<secp256k1_pedersen_commitment*> commitmentPointers = CommitmentUtil::ConvertCommitments(
        *m_context.Read()->Get(),
        commitments
    );

    secp256k1_scratch_space* pScratchSpace = secp256k1_scratch_space_create(
        m_context.Read()->Get(),
        SCRATCH_SPACE_SIZE
    );
    const int result = secp256k1_bulletproof_rangeproof_verify_multi(
        m_context.Read()->Get(),
        pScratchSpace,
        m_pGenerators,
        bulletproofPointers.data(),
        commitments.size(),
        proofLength,
        NULL,
        commitmentPointers.data(),
        1,
        numBits,
        valueGenerators.data(),
        NULL,
        NULL
    );
    secp256k1_scratch_space_destroy(pScratchSpace);

    CommitmentUtil::CleanupCommitments(commitmentPointers);

    if (result == 1)
    {
        for (const Commitment& commitment : commitments)
        {
            m_cache.AddToCache(commitment);
        }
    }

    return result == 1;
}

RangeProof Bulletproofs::GenerateRangeProof(
    const uint64_t amount,
    const SecretKey& key,
    const SecretKey& privateNonce,
    const SecretKey& rewindNonce,
    const ProofMessage& proofMessage)
{
    auto contextWriter = m_context.Write();
    secp256k1_context* pContext = contextWriter->Randomized();

    std::vector<unsigned char> proofBytes(RangeProof::MAX_SIZE, 0);
    size_t proofLen = RangeProof::MAX_SIZE;

    secp256k1_scratch_space* pScratchSpace = secp256k1_scratch_space_create(pContext, SCRATCH_SPACE_SIZE);

    std::vector<const unsigned char*> blindingFactors({ key.data() });
    int result = secp256k1_bulletproof_rangeproof_prove(
        pContext,
        pScratchSpace,
        m_pGenerators,
        &proofBytes[0],
        &proofLen,
        NULL,
        NULL,
        NULL,
        &amount,
        NULL,
        blindingFactors.data(),
        NULL,
        1,
        &secp256k1_generator_const_h,
        64,
        rewindNonce.data(),
        privateNonce.data(),
        NULL,
        0,
        proofMessage.data()
    );
    secp256k1_scratch_space_destroy(pScratchSpace);

    if (result == 1)
    {
        proofBytes.resize(proofLen);
        return RangeProof(std::move(proofBytes));
    }

    throw CryptoEx_F("secp256k1_bulletproof_rangeproof_prove failed with error: {}", result);
}

std::unique_ptr<RewoundProof> Bulletproofs::RewindProof(
    const Commitment& commitment,
    const RangeProof& rangeProof,
    const SecretKey& nonce) const
{
    std::vector<secp256k1_pedersen_commitment*> commitmentPointers = CommitmentUtil::ConvertCommitments(
        *m_context.Read()->Get(),
        std::vector<Commitment>({ commitment })
    );
    if (!commitmentPointers.empty())
    {
        uint64_t value;
        SecretKey blindingFactor;
        std::vector<unsigned char> message(20, 0);

        int result = secp256k1_bulletproof_rangeproof_rewind(
            m_context.Read()->Get(),
            &value,
            blindingFactor.data(),
            rangeProof.data(),
            rangeProof.size(),
            0,
            commitmentPointers.front(),
            &secp256k1_generator_const_h,
            nonce.data(),
            NULL,
            0,
            message.data()
        );
        CommitmentUtil::CleanupCommitments(commitmentPointers);

        if (result == 1)
        {
            return std::make_unique<RewoundProof>(RewoundProof(
                value, 
                std::make_unique<SecretKey>(std::move(blindingFactor)),
                ProofMessage(std::move(message))
            ));
        }
    }

    return std::unique_ptr<RewoundProof>(nullptr);
}