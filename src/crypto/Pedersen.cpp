#include "Pedersen.h"

#include <mw/exceptions/CryptoException.h>
#include <mw/common/Logger.h>

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
            throw CryptoEx("Failed to serialize commitment.");
        }

        return resultCommitment;
    }

    LOG_ERROR_F("Failed to create commitment. Result: {}, Value: {}", result, value);
    throw CryptoEx("Failed to create commitment.");
}

Commitment Pedersen::PedersenCommitSum(const std::vector<Commitment>& positive, const std::vector<Commitment>& negative) const
{
    std::vector<secp256k1_pedersen_commitment*> positiveCommitments = Pedersen::ConvertCommitments(
        *m_context.Read()->Get(),
        positive
    );
    std::vector<secp256k1_pedersen_commitment*> negativeCommitments = Pedersen::ConvertCommitments(
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

    Pedersen::CleanupCommitments(positiveCommitments);
    Pedersen::CleanupCommitments(negativeCommitments);

    if (result != 1)
    {
        LOG_ERROR_F("secp256k1_pedersen_commit_sum returned result: {}", result);
        throw CryptoEx("secp256k1_pedersen_commit_sum error");
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
        throw CryptoEx("secp256k1_pedersen_commitment_serialize error");
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
    throw CryptoEx("secp256k1_pedersen_blind_sum error");
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

    throw CryptoEx_F("secp256k1_blind_switch failed with error: {}", result);
}

std::vector<secp256k1_pedersen_commitment*> Pedersen::ConvertCommitments(const secp256k1_context& context, const std::vector<Commitment>& commitments)
{
    std::vector<secp256k1_pedersen_commitment*> convertedCommitments(commitments.size(), NULL);
    for (int i = 0; i < commitments.size(); i++)
    {
        secp256k1_pedersen_commitment* pCommitment = new secp256k1_pedersen_commitment();
        const int parsed = secp256k1_pedersen_commitment_parse(
            &context,
            pCommitment,
            commitments[i].data()
        );
        convertedCommitments[i] = pCommitment;

        if (parsed != 1)
        {
            CleanupCommitments(convertedCommitments);
            throw CryptoEx_F("secp256k1_pedersen_commitment_parse failed with error: {}", parsed);
        }
    }

    return convertedCommitments;
}

void Pedersen::CleanupCommitments(std::vector<secp256k1_pedersen_commitment*>& commitments)
{
    for (secp256k1_pedersen_commitment* pCommitment : commitments)
    {
        delete pCommitment;
    }

    commitments.clear();
}