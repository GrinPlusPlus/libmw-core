#pragma once

#include "Context.h"

#include <mw/models/crypto/BlindingFactor.h>
#include <mw/models/crypto/SecretKey.h>
#include <mw/models/crypto/Commitment.h>

class Pedersen
{
public:
	Pedersen(Locked<Context>& context) : m_context(context) { }
	~Pedersen() = default;

	Commitment PedersenCommit(
		const uint64_t value,
		const BlindingFactor& blindingFactor
	) const;

	Commitment PedersenCommitSum(
		const std::vector<Commitment>& positive,
		const std::vector<Commitment>& negative
	) const;

	BlindingFactor PedersenBlindSum(
		const std::vector<BlindingFactor>& positive,
		const std::vector<BlindingFactor>& negative
	) const;

	SecretKey BlindSwitch(
		const SecretKey& secretKey,
		const uint64_t amount
	) const;

	static std::vector<secp256k1_pedersen_commitment*> ConvertCommitments(
		const secp256k1_context& context,
		const std::vector<Commitment>& commitments
	);

	static void CleanupCommitments(
		std::vector<secp256k1_pedersen_commitment*>& commitments
	);

private:
	Locked<Context> m_context;
};