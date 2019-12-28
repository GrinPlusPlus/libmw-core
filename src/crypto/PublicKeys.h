#pragma once

#include "Context.h"

#include <mw/core/models/crypto/SecretKey.h>
#include <mw/core/models/crypto/PublicKey.h>

class PublicKeys
{
public:
    PublicKeys(Locked<Context>& context) : m_context(context) { }
    ~PublicKeys() = default;

    PublicKey CalculatePublicKey(const SecretKey& privateKey) const;
    PublicKey PublicKeySum(const std::vector<PublicKey>& publicKeys) const;

private:
    std::vector<secp256k1_pubkey*> ParsePubKeys(const std::vector<PublicKey>& publicKeys) const;

    Locked<Context> m_context;
};