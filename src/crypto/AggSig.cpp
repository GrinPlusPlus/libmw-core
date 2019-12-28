#include "AggSig.h"
#include "Pedersen.h"

#include<mw/core/common/Logger.h>
#include<mw/core/crypto/Random.h>
#include<mw/core/exceptions/CryptoException.h>

const uint64_t MAX_WIDTH = 1 << 20;
const size_t SCRATCH_SPACE_SIZE = 256 * MAX_WIDTH;

SecretKey AggSig::GenerateSecureNonce() const
{
    auto context = m_context.Read();

    SecretKey nonce;
    const SecretKey seed = Random::CSPRNG<32>();
    const int result = secp256k1_aggsig_export_secnonce_single(
        context->Get(),
        nonce.data(),
        seed.data()
    );
    if (result == 1)
    {
        return nonce;
    }

    throw CryptoEx_F("secp256k1_aggsig_export_secnonce_single failed with error: {}", result);
}

CompactSignature::UPtr AggSig::SignMessage(
    const SecretKey& secretKey,
    const PublicKey& publicKey,
    const Hash& message)
{
    auto context = m_context.Write();

    secp256k1_pubkey pubKey;
    const int pubKeyParsed = secp256k1_ec_pubkey_parse(
        context->Get(),
        &pubKey,
        publicKey.data(),
        publicKey.size()
    );
    if (pubKeyParsed == 1)
    {
        const SecretKey randomSeed = Random::CSPRNG<32>();

        secp256k1_ecdsa_signature signature;
        const int signedResult = secp256k1_aggsig_sign_single(
            context->Randomized(),
            &signature.data[0],
            message.data(),
            secretKey.data(),
            nullptr,
            nullptr,
            nullptr,
            nullptr,
            &pubKey,
            randomSeed.data()
        );

        if (signedResult == 1)
        {
            CompactSignature sig64;
            const int serializedResult = secp256k1_ecdsa_signature_serialize_compact(
                context->Get(),
                sig64.data(),
                &signature
            );
            if (serializedResult == 1)
            {
                return std::make_unique<CompactSignature>(std::move(sig64));
            }
        }
    }

    return nullptr;
}

bool AggSig::VerifyMessageSignature(
    const CompactSignature& signature,
    const PublicKey& publicKey,
    const Hash& message) const
{
    auto contextReader = m_context.Read();
    const secp256k1_context* pContext = contextReader->Get();

    secp256k1_ecdsa_signature secpSig;
    const int parseSignatureResult = secp256k1_ecdsa_signature_parse_compact(
        pContext,
        &secpSig,
        signature.data()
    );
    if (parseSignatureResult == 1)
    {
        secp256k1_pubkey pubkey;
        const int pubkeyResult = secp256k1_ec_pubkey_parse(
            pContext,
            &pubkey,
            publicKey.data(),
            publicKey.size()
        );
        if (pubkeyResult == 1)
        {
            const int verifyResult = secp256k1_aggsig_verify_single(
                pContext,
                secpSig.data,
                message.data(),
                nullptr,
                &pubkey,
                &pubkey,
                nullptr,
                false
            );
            if (verifyResult == 1)
            {
                return true;
            }
        }
    }

    return false;
}

CompactSignature::UPtr AggSig::CalculatePartialSignature(
    const SecretKey& secretKey,
    const SecretKey& secretNonce,
    const PublicKey& sumPubKeys,
    const PublicKey& sumPubNonces,
    const Hash& message)
{
    auto context = m_context.Write();
    secp256k1_context* pContext = context->Randomized();

    secp256k1_pubkey pubKeyForE;
    int pubKeyParsed = secp256k1_ec_pubkey_parse(
        pContext,
        &pubKeyForE,
        sumPubKeys.data(),
        sumPubKeys.size()
    );

    secp256k1_pubkey pubNoncesForE;
    int noncesParsed = secp256k1_ec_pubkey_parse(
        pContext,
        &pubNoncesForE,
        sumPubNonces.data(),
        sumPubNonces.size()
    );

    if (pubKeyParsed == 1 && noncesParsed == 1)
    {
        const SecretKey randomSeed = Random::CSPRNG<32>();

        secp256k1_ecdsa_signature signature;
        const int signedResult = secp256k1_aggsig_sign_single(
            pContext,
            &signature.data[0],
            message.data(),
            secretKey.data(),
            secretNonce.data(),
            nullptr,
            &pubNoncesForE,
            &pubNoncesForE,
            &pubKeyForE,
            randomSeed.data()
        );

        if (signedResult == 1)
        {
            CompactSignature sig64;
            const int serializedResult = secp256k1_ecdsa_signature_serialize_compact(
                pContext,
                sig64.data(),
                &signature
            );
            if (serializedResult == 1)
            {
                return std::make_unique<CompactSignature>(std::move(sig64));
            }
        }
    }

    return std::unique_ptr<CompactSignature>(nullptr);
}

bool AggSig::VerifyPartialSignature(
    const CompactSignature& partialSignature,
    const PublicKey& publicKey,
    const PublicKey& sumPubKeys,
    const PublicKey& sumPubNonces,
    const Hash& message) const
{
    secp256k1_ecdsa_signature signature;

    const int parseSignatureResult = secp256k1_ecdsa_signature_parse_compact(
        m_context.Read()->Get(),
        &signature,
        partialSignature.data()
    );
    if (parseSignatureResult == 1)
    {
        secp256k1_pubkey pubkey;
        const int pubkeyResult = secp256k1_ec_pubkey_parse(
            m_context.Read()->Get(),
            &pubkey,
            publicKey.data(),
            publicKey.size()
        );

        secp256k1_pubkey sumPubKey;
        const int sumPubkeysResult = secp256k1_ec_pubkey_parse(
            m_context.Read()->Get(),
            &sumPubKey,
            sumPubKeys.data(),
            sumPubKeys.size()
        );

        secp256k1_pubkey sumNoncesPubKey;
        const int sumPubNonceKeyResult = secp256k1_ec_pubkey_parse(
            m_context.Read()->Get(),
            &sumNoncesPubKey,
            sumPubNonces.data(),
            sumPubNonces.size()
        );

        if (pubkeyResult == 1 && sumPubkeysResult == 1 && sumPubNonceKeyResult == 1)
        {
            const int verifyResult = secp256k1_aggsig_verify_single(
                m_context.Read()->Get(),
                signature.data,
                message.data(),
                &sumNoncesPubKey,
                &pubkey,
                &sumPubKey,
                nullptr,
                true
            );
            if (verifyResult == 1)
            {
                return true;
            }
        }
    }

    return false;
}

Signature::UPtr AggSig::AggregateSignatures(
    const std::vector<CompactSignature>& signatures,
    const PublicKey& sumPubNonces) const
{
    auto contextReader = m_context.Read();

    secp256k1_pubkey pubNonces;
    const int noncesParsed = secp256k1_ec_pubkey_parse(
        m_context.Read()->Get(),
        &pubNonces,
        sumPubNonces.data(),
        sumPubNonces.size()
    );
    if (noncesParsed == 1)
    {
        std::vector<secp256k1_ecdsa_signature> parsedSignatures = ParseCompactSignatures(
            m_context.Read()->Get(),
            signatures
        );
        if (!parsedSignatures.empty())
        {
            std::vector<secp256k1_ecdsa_signature*> signaturePointers;
            std::transform(
                parsedSignatures.begin(),
                parsedSignatures.end(),
                std::back_inserter(signaturePointers),
                [](secp256k1_ecdsa_signature& parsedSig) { return &parsedSig; }
            );

            secp256k1_ecdsa_signature aggregatedSignature;
            const int result = secp256k1_aggsig_add_signatures_single(
                m_context.Read()->Get(),
                aggregatedSignature.data,
                (const unsigned char**)signaturePointers.data(),
                signaturePointers.size(),
                &pubNonces
            );

            if (result == 1)
            {
                return std::make_unique<Signature>(aggregatedSignature.data);
            }
        }
    }

    LOG_ERROR("Failed to aggregate signatures");
    return std::unique_ptr<Signature>(nullptr);
}

bool AggSig::VerifyAggregateSignatures(
    const std::vector<const Signature*>& signatures,
    const std::vector<const Commitment*>& commitments,
    const std::vector<const Hash*>& messages) const
{
    std::vector<secp256k1_pubkey> parsedPubKeys;
    for (const Commitment* commitment : commitments)
    {
        secp256k1_pedersen_commitment parsedCommitment;
        const int commitmentResult = secp256k1_pedersen_commitment_parse(
            m_context.Read()->Get(),
            &parsedCommitment,
            commitment->data()
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
                parsedPubKeys.emplace_back(std::move(pubKey));
            }
            else
            {
                LOG_ERROR_F("Failed to convert commitment ({}) to pubkey", *commitment);
                return false;
            }
        }
        else
        {
            LOG_ERROR_F("Failed to parse commitment: {}", *commitment);
            return false;
        }
    }

    std::vector<secp256k1_pubkey*> pubKeyPtrs(parsedPubKeys.size());
    for (size_t i = 0; i < parsedPubKeys.size(); i++)
    {
        pubKeyPtrs[i] = &parsedPubKeys[i];
    }

    std::vector<secp256k1_schnorrsig> parsedSignatures;
    for (const Signature* signature : signatures)
    {
        secp256k1_schnorrsig parsedSig;
        if (secp256k1_schnorrsig_parse(m_context.Read()->Get(), &parsedSig, signature->data()) == 0)
        {
            return false;
        }

        parsedSignatures.emplace_back(std::move(parsedSig));
    }

    std::vector<const secp256k1_schnorrsig*> signaturePtrs;
    std::transform(
        parsedSignatures.cbegin(),
        parsedSignatures.cend(),
        std::back_inserter(signaturePtrs),
        [](const secp256k1_schnorrsig& parsedSig) { return &parsedSig; }
    );

    std::vector<const unsigned char*> messageData;
    std::transform(
        messages.cbegin(),
        messages.cend(),
        std::back_inserter(messageData),
        [](const Hash* pMessage) { return pMessage->data(); }
    );

    secp256k1_scratch_space* pScratchSpace = secp256k1_scratch_space_create(
        m_context.Read()->Get(),
        SCRATCH_SPACE_SIZE
    );
    const int verifyResult = secp256k1_schnorrsig_verify_batch(
        m_context.Read()->Get(),
        pScratchSpace,
        signaturePtrs.data(),
        messageData.data(),
        pubKeyPtrs.data(),
        signatures.size()
    );
    secp256k1_scratch_space_destroy(pScratchSpace);

    if (verifyResult == 1)
    {
        return true;
    }
    else
    {
        LOG_ERROR("Signature failed to verify.");
        return false;
    }
}

bool AggSig::VerifyAggregateSignature(
    const Signature& signature,
    const PublicKey& sumPubKeys,
    const Hash& message) const
{
    secp256k1_pubkey parsedPubKey;
    const int parseResult = secp256k1_ec_pubkey_parse(
        m_context.Read()->Get(),
        &parsedPubKey,
        sumPubKeys.data(),
        sumPubKeys.size()
    );
    if (parseResult == 1)
    {
        const int verifyResult = secp256k1_aggsig_verify_single(
            m_context.Read()->Get(),
            signature.data(),
            message.data(),
            nullptr,
            &parsedPubKey,
            &parsedPubKey,
            nullptr,
            false
        );
        if (verifyResult == 1)
        {
            return true;
        }
    }

    return false;
}

std::vector<secp256k1_ecdsa_signature> AggSig::ParseCompactSignatures(
    const secp256k1_context* pContext,
    const std::vector<CompactSignature>& signatures) const
{
    std::vector<secp256k1_ecdsa_signature> parsed;
    for (const Signature partialSignature : signatures)
    {
        secp256k1_ecdsa_signature signature;
        const int parseSignatureResult = secp256k1_ecdsa_signature_parse_compact(
            pContext,
            &signature,
            partialSignature.data()
        );
        if (parseSignatureResult == 1)
        {
            parsed.emplace_back(std::move(signature));
        }
        else
        {
            return std::vector<secp256k1_ecdsa_signature>();
        }
    }

    return parsed;
}