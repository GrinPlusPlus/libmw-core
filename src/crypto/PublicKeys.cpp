#include "PublicKeys.h"

#include <mw/exceptions/CryptoException.h>

PublicKey PublicKeys::CalculatePublicKey(const SecretKey& privateKey) const
{
    const int verifyResult = secp256k1_ec_seckey_verify(m_context.Read()->Get(), privateKey.data());
    if (verifyResult == 1)
    {
        secp256k1_pubkey pubkey;
        const int createResult = secp256k1_ec_pubkey_create(
            m_context.Read()->Get(),
            &pubkey,
            privateKey.data()
        );
        if (createResult == 1)
        {
            PublicKey result;
            size_t length = result.size();
            const int serializeResult = secp256k1_ec_pubkey_serialize(
                m_context.Read()->Get(),
                result.data(),
                &length,
                &pubkey,
                SECP256K1_EC_COMPRESSED
            );
            if (serializeResult == 1)
            {
                return result;
            }
        }
    }

    throw CryptoEx("Failed to calculate public key");
}

PublicKey PublicKeys::PublicKeySum(const std::vector<PublicKey>& publicKeys) const
{
    std::vector<secp256k1_pubkey*> parsedPubKeys = ParsePubKeys(publicKeys);

    secp256k1_pubkey publicKey;
    const int pubKeysCombined = secp256k1_ec_pubkey_combine(
        m_context.Read()->Get(),
        &publicKey,
        parsedPubKeys.data(),
        parsedPubKeys.size()
    );
    for (secp256k1_pubkey* pParsedPubKey : parsedPubKeys)
    {
        delete pParsedPubKey;
    }

    if (pubKeysCombined == 1)
    {
        PublicKey result;
        size_t length = result.size();
        const int serializeResult = secp256k1_ec_pubkey_serialize(
            m_context.Read()->Get(),
            result.data(),
            &length,
            &publicKey,
            SECP256K1_EC_COMPRESSED
        );
        if (serializeResult == 1)
        {
            return result;
        }
    }

    throw CryptoEx("Failed to combine public keys.");
}

std::vector<secp256k1_pubkey*> PublicKeys::ParsePubKeys(const std::vector<PublicKey>& publicKeys) const
{
    std::vector<secp256k1_pubkey*> parsedPubKeys;
    for (const PublicKey& publicKey : publicKeys)
    {
        secp256k1_pubkey* pPublicKey = new secp256k1_pubkey();
        int pubKeyParsed = secp256k1_ec_pubkey_parse(
            m_context.Read()->Get(),
            pPublicKey,
            publicKey.data(),
            publicKey.size()
        );
        if (pubKeyParsed == 1)
        {
            parsedPubKeys.push_back(pPublicKey);
        }
        else
        {
            delete pPublicKey;
            for (secp256k1_pubkey* pParsedPubKey : parsedPubKeys)
            {
                delete pParsedPubKey;
            }

            throw CryptoEx_F("secp256k1_ec_pubkey_parse failed with error: {}", pubKeyParsed);
        }
    }

    return parsedPubKeys;
}