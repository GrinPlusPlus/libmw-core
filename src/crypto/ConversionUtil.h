#pragma once

#include "Context.h"

#include <mw/core/exceptions/CryptoException.h>
#include <mw/core/models/crypto/Commitment.h>
#include <mw/core/models/crypto/PublicKey.h>

class ConversionUtil
{
public:
    ConversionUtil(const Locked<Context>& context) : m_context(context) { }

    PublicKey ToPublicKey(const Commitment& commitment)
    {
        secp256k1_pedersen_commitment parsedCommitment;
        const int commitmentResult = secp256k1_pedersen_commitment_parse(
            m_context.Read()->Get(),
            &parsedCommitment,
            commitment.data()
        );
        if (commitmentResult == 1)
        {
            secp256k1_pubkey pubKey;
            const int pubkeyResult = secp256k1_pedersen_commitment_to_pubkey(
                m_context.Read()->Get(),
                &pubKey,
                &parsedCommitment
            );

            if (pubkeyResult == 1)
            {
                PublicKey result;
                size_t length = result.size();
                const int serializeResult = secp256k1_ec_pubkey_serialize(
                    m_context.Read()->Get(),
                    result.data(),
                    &length,
                    &pubKey,
                    SECP256K1_EC_COMPRESSED
                );
                if (serializeResult == 1)
                {
                    return result;
                }
            }

            ThrowCrypto_F("Failed to convert commitment ({}) to pubkey", commitment);
        }

        ThrowCrypto_F("Failed to parse commitment: {}", commitment);
    }

private:
    Locked<Context> m_context;
};