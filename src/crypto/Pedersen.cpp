#include "Pedersen.h"
#include "CommitmentUtil.h"

#include <mw/core/exceptions/CryptoException.h>
#include <mw/core/common/Logger.h>

Commitment Pedersen::PedersenCommit(const uint64_t value, const BlindingFactor& blindingFactor) const
{
    secp256k1_pedersen_commitment commitment;
    const int result = secp256k1_pedersen_commit(
        m_context.Read()->Get(),
        &commitment,
        blindingFactor.data(),
        value,
        &secp256k1_generator_const_h,
        &secp256k1_generator_const_g
    );
    if (result == 1)
    {
        Commitment resultCommitment;
        const int serializedResult = secp256k1_pedersen_commitment_serialize(
            m_context.Read()->Get(),
            resultCommitment.data(),
            &commitment
        );
        if (serializedResult != 1)
        {
            LOG_ERROR_F("Failed to serialize commitment. Result: {}", serializedResult);
            ThrowCrypto("Failed to serialize commitment.");
        }

        return resultCommitment;
    }

    LOG_ERROR_F("Failed to create commitment. Result: {}, Value: {}", result, value);
    ThrowCrypto("Failed to create commitment.");
}

Commitment Pedersen::PedersenCommitSum(const std::vector<Commitment>& positive, const std::vector<Commitment>& negative) const
{
    std::vector<secp256k1_pedersen_commitment*> positiveCommitments = CommitmentUtil::ConvertCommitments(
        *m_context.Read()->Get(),
        positive
    );
    std::vector<secp256k1_pedersen_commitment*> negativeCommitments = CommitmentUtil::ConvertCommitments(
        *m_context.Read()->Get(),
        negative
    );

    secp256k1_pedersen_commitment commitment;
    const int result = secp256k1_pedersen_commit_sum(
        m_context.Read()->Get(),
        &commitment,
        positiveCommitments.empty() ? nullptr : &positiveCommitments[0],
        positiveCommitments.size(),
        negativeCommitments.empty() ? nullptr : &negativeCommitments[0],
        negativeCommitments.size()
    );

    CommitmentUtil::CleanupCommitments(positiveCommitments);
    CommitmentUtil::CleanupCommitments(negativeCommitments);

    if (result != 1)
    {
        LOG_ERROR_F("secp256k1_pedersen_commit_sum returned result: {}", result);
        ThrowCrypto("secp256k1_pedersen_commit_sum error");
    }

    Commitment resultCommitment;
    const int serializeResult = secp256k1_pedersen_commitment_serialize(
        m_context.Read()->Get(),
        resultCommitment.data(),
        &commitment
    );
    if (serializeResult != 1)
    {
        LOG_ERROR_F("secp256k1_pedersen_commitment_serialize returned result: {}", serializeResult);
        ThrowCrypto("secp256k1_pedersen_commitment_serialize error");
    }

    return resultCommitment;
}

BlindingFactor Pedersen::PedersenBlindSum(const std::vector<BlindingFactor>& positive, const std::vector<BlindingFactor>& negative) const
{
    std::vector<const unsigned char*> blindingFactors;
    for (const BlindingFactor& positiveFactor : positive)
    {
        blindingFactors.push_back(positiveFactor.data());
    }

    for (const BlindingFactor& negativeFactor : negative)
    {
        blindingFactors.push_back(negativeFactor.data());
    }

    BlindingFactor blindingFactor;
    const int result = secp256k1_pedersen_blind_sum(
        m_context.Read()->Get(),
        blindingFactor.data(),
        blindingFactors.data(),
        blindingFactors.size(),
        positive.size()
    );
    if (result == 1)
    {
        return blindingFactor;
    }

    LOG_ERROR_F("secp256k1_pedersen_blind_sum returned result: {}", result);
    ThrowCrypto("secp256k1_pedersen_blind_sum error");
}

SecretKey Pedersen::BlindSwitch(const SecretKey& blindingFactor, const uint64_t amount) const
{
    SecretKey blindSwitch;
    const int result = secp256k1_blind_switch(
        m_context.Read()->Get(),
        blindSwitch.data(),
        blindingFactor.data(),
        amount,
        &secp256k1_generator_const_h,
        &secp256k1_generator_const_g,
        &GENERATOR_J_PUB
    );
    if (result == 1)
    {
        return blindSwitch;
    }

    ThrowCrypto_F("secp256k1_blind_switch failed with error: {}", result);
}