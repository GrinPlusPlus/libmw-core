#pragma once

#include <mw/core/crypto/secp256k1.h>

class CommitmentUtil
{
public:
	static std::vector<secp256k1_pedersen_commitment*> ConvertCommitments(
		const secp256k1_context& context,
		const std::vector<Commitment>& commitments)
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

    static void CleanupCommitments(std::vector<secp256k1_pedersen_commitment*>& commitments)
    {
        for (secp256k1_pedersen_commitment* pCommitment : commitments)
        {
            delete pCommitment;
        }

        commitments.clear();
    }
};