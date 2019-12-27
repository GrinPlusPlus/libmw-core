#include <mw/crypto/Crypto.h>
#include <mw/exceptions/CryptoException.h>
#include <mw/common/Logger.h>

#include <Crypto/Blake2.h>
#include <Crypto/sha256.h>
#include <Crypto/ripemd160.h>
#include <Crypto/hmac_sha256.h>
#include <Crypto/hmac_sha512.h>
#include <Crypto/aes.h>
#include <Crypto/siphash.h>
#include <Crypto/crypto_scrypt.h>

// Secp256k1
#include "Context.h"
#include "AggSig.h"
#include "Bulletproofs.h"
#include "Pedersen.h"
#include "PublicKeys.h"

#ifdef _WIN32
#pragma comment(lib, "crypt32")
#endif

Locked<Context> SECP256K1_CONTEXT(std::make_shared<Context>());

BigInt<32> Crypto::Blake2b(const std::vector<unsigned char>& input)
{
    std::vector<unsigned char> tmp(32, 0);

    blake2b(&tmp[0], 32, &input[0], input.size(), nullptr, 0);

    return BigInt<32>(&tmp[0]);
}

BigInt<32> Crypto::Blake2b(const std::vector<unsigned char>& key, const std::vector<unsigned char>& input)
{
    std::vector<unsigned char> tmp(32, 0);

    blake2b(&tmp[0], 32, input.data(), input.size(), key.data(), key.size());

    return BigInt<32>(&tmp[0]);
}

BigInt<32> Crypto::SHA256(const std::vector<unsigned char>& input)
{
    std::vector<unsigned char> sha256(32, 0);

    CSHA256().Write(input.data(), input.size()).Finalize(sha256.data());

    return BigInt<32>(sha256);
}

BigInt<64> Crypto::SHA512(const std::vector<unsigned char>& input)
{
    std::vector<unsigned char> sha512(64, 0);

    CSHA512().Write(input.data(), input.size()).Finalize(sha512.data());

    return BigInt<64>(sha512);
}

BigInt<20> Crypto::RipeMD160(const std::vector<unsigned char>& input)
{
    std::vector<unsigned char> ripemd(32, 0);

    CRIPEMD160().Write(input.data(), input.size()).Finalize(ripemd.data());

    return BigInt<20>(ripemd);
}

BigInt<32> Crypto::HMAC_SHA256(const std::vector<unsigned char>& key, const std::vector<unsigned char>& data)
{
    std::vector<unsigned char> result(32);

    CHMAC_SHA256(key.data(), key.size()).Write(data.data(), data.size()).Finalize(result.data());

    return BigInt<32>(result.data());
}

BigInt<64> Crypto::HMAC_SHA512(const std::vector<unsigned char>& key, const std::vector<unsigned char>& data)
{
    std::vector<unsigned char> result(64);

    CHMAC_SHA512(key.data(), key.size()).Write(data.data(), data.size()).Finalize(result.data());

    return BigInt<64>(std::move(result));
}

Commitment Crypto::CommitTransparent(const uint64_t value)
{
    const BlindingFactor blindingFactor(BigInt<32>::ValueOf(0));

    return Pedersen(SECP256K1_CONTEXT).PedersenCommit(value, blindingFactor);
}

Commitment Crypto::CommitBlinded(const uint64_t value, const BlindingFactor& blindingFactor)
{
    return Pedersen(SECP256K1_CONTEXT).PedersenCommit(value, blindingFactor);
}

Commitment Crypto::AddCommitments(const std::vector<Commitment>& positive, const std::vector<Commitment>& negative)
{
    const Commitment zeroCommitment(BigInt<33>::ValueOf(0));

    std::vector<Commitment> sanitizedPositive;
    std::copy_if(
        positive.cbegin(),
        positive.cend(),
        std::back_inserter(sanitizedPositive),
        [&zeroCommitment](const Commitment& positiveCommitment) { return positiveCommitment != zeroCommitment; }
    );

    std::vector<Commitment> sanitizedNegative;
    std::copy_if(
        negative.cbegin(),
        negative.cend(),
        std::back_inserter(sanitizedNegative),
        [&zeroCommitment](const Commitment& negativeCommitment) { return negativeCommitment != zeroCommitment; }
    );

    return Pedersen(SECP256K1_CONTEXT).PedersenCommitSum(sanitizedPositive, sanitizedNegative);
}

BlindingFactor Crypto::AddBlindingFactors(const std::vector<BlindingFactor>& positive, const std::vector<BlindingFactor>& negative)
{
    BlindingFactor zeroBlindingFactor(ZERO_HASH);

    std::vector<BlindingFactor> sanitizedPositive;
    std::copy_if(
        positive.cbegin(),
        positive.cend(),
        std::back_inserter(sanitizedPositive),
        [&zeroBlindingFactor](const BlindingFactor& positiveBlind) { return positiveBlind != zeroBlindingFactor; }
    );

    std::vector<BlindingFactor> sanitizedNegative;
    std::copy_if(
        negative.cbegin(),
        negative.cend(),
        std::back_inserter(sanitizedNegative),
        [&zeroBlindingFactor](const BlindingFactor& negativeBlind) { return negativeBlind != zeroBlindingFactor; }
    );

    if (sanitizedPositive.empty() && sanitizedNegative.empty())
    {
        return zeroBlindingFactor;
    }

    return Pedersen(SECP256K1_CONTEXT).PedersenBlindSum(sanitizedPositive, sanitizedNegative);
}

SecretKey Crypto::BlindSwitch(const SecretKey& secretKey, const uint64_t amount)
{
    return Pedersen(SECP256K1_CONTEXT).BlindSwitch(secretKey, amount);
}

SecretKey Crypto::AddPrivateKeys(const SecretKey& secretKey1, const SecretKey& secretKey2)
{
    SecretKey result(secretKey1.vec());

    const int tweakResult = secp256k1_ec_privkey_tweak_add(
        SECP256K1_CONTEXT.Read()->Get(),
        (unsigned char*)result.data(),
        secretKey2.data()
    );
    if (tweakResult == 1)
    {
        return result;
    }

    throw CryptoEx("secp256k1_ec_privkey_tweak_add failed");
}

RangeProof Crypto::GenerateRangeProof(const uint64_t amount, const SecretKey& key, const SecretKey& privateNonce, const SecretKey& rewindNonce, const ProofMessage& proofMessage)
{
    return Bulletproofs(SECP256K1_CONTEXT).GenerateRangeProof(amount, key, privateNonce, rewindNonce, proofMessage);
}

std::unique_ptr<RewoundProof> Crypto::RewindRangeProof(const Commitment& commitment, const RangeProof& rangeProof, const SecretKey& nonce)
{
    return Bulletproofs(SECP256K1_CONTEXT).RewindProof(commitment, rangeProof, nonce);
}

bool Crypto::VerifyRangeProofs(const std::vector<std::pair<Commitment, RangeProof>>& rangeProofs)
{
    return Bulletproofs(SECP256K1_CONTEXT).VerifyBulletproofs(rangeProofs);
}

uint64_t Crypto::SipHash24(const uint64_t k0, const uint64_t k1, const std::vector<unsigned char>& data)
{
    const std::vector<uint64_t>& key = { k0, k1 };

    return siphash24(&key[0], &data[0], data.size());
}

std::vector<unsigned char> Crypto::AES256_Encrypt(const SecureVector& input, const SecretKey& key, const BigInt<16>& iv)
{
    std::vector<unsigned char> ciphertext;

    // max ciphertext len for a n bytes of plaintext is n + AES_BLOCKSIZE bytes
    ciphertext.resize(input.size() + AES_BLOCKSIZE);

    AES256CBCEncrypt enc(key.data(), iv.data(), true);
    const size_t nLen = enc.Encrypt(&input[0], (int)input.size(), ciphertext.data());
    if (nLen < input.size())
    {
        throw CryptoEx("Failed to encrypt");
    }

    ciphertext.resize(nLen);

    return ciphertext;
}

SecureVector Crypto::AES256_Decrypt(const std::vector<unsigned char>& ciphertext, const SecretKey& key, const BigInt<16>& iv)
{
    SecureVector plaintext;

    // plaintext will always be equal to or lesser than length of ciphertext
    size_t nLen = ciphertext.size();

    plaintext.resize(nLen);

    AES256CBCDecrypt dec(key.data(), iv.data(), true);
    nLen = dec.Decrypt(ciphertext.data(), (int)ciphertext.size(), plaintext.data());
    if (nLen == 0)
    {
        throw CryptoEx("Failed to decrypt");
    }

    plaintext.resize(nLen);

    return plaintext;
}

SecretKey Crypto::PBKDF(const SecureString& password, const std::vector<unsigned char>& salt, const ScryptParameters& parameters)
{
    SecureVector buffer(64);
    const int result = crypto_scrypt(
        (const unsigned char*)password.data(),
        password.size(),
        salt.data(),
        salt.size(),
        parameters.N,
        parameters.r,
        parameters.p,
        buffer.data(),
        buffer.size()
    );
    if (result == 0)
    {
        std::vector<unsigned char> tmp(32, 0);

        blake2b(&tmp[0], 32, &buffer[0], buffer.size(), nullptr, 0);

        return SecretKey(BigInt<32>(&tmp[0]));
    }

    throw CryptoEx("Scrypt failed");
}

PublicKey Crypto::CalculatePublicKey(const SecretKey& privateKey)
{
    return PublicKeys(SECP256K1_CONTEXT).CalculatePublicKey(privateKey);
}

PublicKey Crypto::AddPublicKeys(const std::vector<PublicKey>& publicKeys)
{
    return PublicKeys(SECP256K1_CONTEXT).PublicKeySum(publicKeys);
}

CompactSignature::UPtr Crypto::SignMessage(const SecretKey& secretKey, const PublicKey& publicKey, const std::string& message)
{
    const Hash messageHash = Crypto::Blake2b(std::vector<unsigned char>(message.cbegin(), message.cend()));
    return AggSig(SECP256K1_CONTEXT).SignMessage(secretKey, publicKey, messageHash);
}

bool Crypto::VerifyMessageSignature(const CompactSignature& signature, const PublicKey& publicKey, const std::string& message)
{
    const Hash messageHash = Crypto::Blake2b(std::vector<unsigned char>(message.cbegin(), message.cend()));
    return AggSig(SECP256K1_CONTEXT).VerifyMessageSignature(signature, publicKey, messageHash);
}

CompactSignature::UPtr Crypto::CalculatePartialSignature(const SecretKey& secretKey, const SecretKey& secretNonce, const PublicKey& sumPubKeys, const PublicKey& sumPubNonces, const Hash& message)
{
    return AggSig(SECP256K1_CONTEXT).CalculatePartialSignature(secretKey, secretNonce, sumPubKeys, sumPubNonces, message);
}

Signature::UPtr Crypto::AggregateSignatures(const std::vector<CompactSignature>& signatures, const PublicKey& sumPubNonces)
{
    return AggSig(SECP256K1_CONTEXT).AggregateSignatures(signatures, sumPubNonces);
}

bool Crypto::VerifyPartialSignature(const CompactSignature& partialSignature, const PublicKey& publicKey, const PublicKey& sumPubKeys, const PublicKey& sumPubNonces, const Hash& message)
{
    return AggSig(SECP256K1_CONTEXT).VerifyPartialSignature(partialSignature, publicKey, sumPubKeys, sumPubNonces, message);
}

bool Crypto::VerifyAggregateSignature(const Signature& aggregateSignature, const PublicKey sumPubKeys, const Hash& message)
{
    return AggSig(SECP256K1_CONTEXT).VerifyAggregateSignature(aggregateSignature, sumPubKeys, message);
}

bool Crypto::VerifyKernelSignatures(const std::vector<const Signature*>& signatures, const std::vector<const Commitment*>& publicKeys, const std::vector<const Hash*>& messages)
{
    return AggSig(SECP256K1_CONTEXT).VerifyAggregateSignatures(signatures, publicKeys, messages);
}

SecretKey Crypto::GenerateSecureNonce()
{
    return AggSig(SECP256K1_CONTEXT).GenerateSecureNonce();
}