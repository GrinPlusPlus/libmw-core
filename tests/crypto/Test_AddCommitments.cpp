#include <catch.hpp>

#include <mw/crypto/secp256k1.h>
#include <mw/crypto/Crypto.h>
#include <mw/crypto/Random.h>

TEST_CASE("Crypto::AddCommitment")
{
    // Test adding blinded commitment with transparent one
    {
        BlindingFactor blind_a = Random::CSPRNG<32>().GetBigInt() / 2;

        Commitment commit_a = Crypto::CommitBlinded(3, blind_a);
        Commitment commit_b = Crypto::CommitTransparent(2);

        Commitment sum = Crypto::AddCommitments(
            std::vector<Commitment>({ commit_a, commit_b }),
            std::vector<Commitment>()
        );
        Commitment expected = Crypto::CommitBlinded(5, blind_a);

        REQUIRE(sum == expected);
    }

    // Test adding 2 blinded commitment
    {
        BlindingFactor blind_a = Random::CSPRNG<32>().GetBigInt() / 2;
        BlindingFactor blind_b = Random::CSPRNG<32>().GetBigInt() / 2;

        Commitment commit_a = Crypto::CommitBlinded(3, blind_a);
        Commitment commit_b = Crypto::CommitBlinded(2, blind_b);
        Commitment sum = Crypto::AddCommitments(
            std::vector<Commitment>({ commit_a, commit_b }),
            std::vector<Commitment>()
        );

        secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);

        std::vector<unsigned char> blindOutBytes(32);
        std::vector<const unsigned char*> blindingIn({ blind_a.data(), blind_b.data() });
        secp256k1_pedersen_blind_sum(ctx, blindOutBytes.data(), blindingIn.data(), 2, 2);

        BlindingFactor blind_c(std::move(blindOutBytes));
        Commitment commit_c = Crypto::CommitBlinded(5, blind_c);
        REQUIRE(commit_c == sum);
    }

    // Test adding negative blinded commitment
    {
        BlindingFactor blind_a = Random::CSPRNG<32>().GetBigInt() / 2;
        BlindingFactor blind_b = Random::CSPRNG<32>().GetBigInt() / 2;

        Commitment commit_a = Crypto::CommitBlinded(3, blind_a);
        Commitment commit_b = Crypto::CommitBlinded(2, blind_b);
        Commitment difference = Crypto::AddCommitments(
            std::vector<Commitment>({ commit_a }),
            std::vector<Commitment>({ commit_b })
        );

        secp256k1_context* ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);

        std::vector<unsigned char> blindOutBytes(32);
        std::vector<const unsigned char*> blindingIn({ blind_a.data(), blind_b.data() });
        secp256k1_pedersen_blind_sum(ctx, blindOutBytes.data(), blindingIn.data(), 2, 1);

        BlindingFactor blind_c(std::move(blindOutBytes));
        Commitment commit_c = Crypto::CommitBlinded(1, blind_c);
        REQUIRE(commit_c == difference);
    }
}