#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/core/common/Secure.h>
#include <mw/core/models/crypto/SecretKey.h>
#include <mw/core/exceptions/CryptoException.h>

#include <random>

#ifdef _WIN32
#pragma comment(lib, "Bcrypt.lib")
#include <bcrypt.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif

//
// A wrapper around a third-party random number generators.
//
class Random
{
public:
    template<size_t NUM_BYTES>
    static secret_key_t<NUM_BYTES> CSPRNG()
    {
        return GenerateRandomBytes(NUM_BYTES);
    }

    static uint64_t FastRandom(const uint64_t minimum, const uint64_t maximum)
    {
        std::random_device seeder;
        std::mt19937 engine(seeder());
        std::uniform_int_distribution<uint64_t> dist(minimum, maximum);

        return dist(engine);
    }

private:
    static SecureVector GenerateRandomBytes(const size_t numBytes)
    {
        SecureVector buffer(numBytes);
#ifdef _WIN32
        const NTSTATUS status = BCryptGenRandom(nullptr, buffer.data(), (ULONG)buffer.size(), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        if (!BCRYPT_SUCCESS(status))
        {
            ThrowCrypto("RNG Failure");
        }
#else
        bool success = false;
        int hFile = open("/dev/urandom", O_RDONLY);
        if (hFile >= 0)
        {
            if (read(hFile, buffer.data(), numBytes) == numBytes)
            {
                success = true;
            }

            close(hFile);
        }

        if (!success)
        {
            ThrowCrypto("RNG Failure");
        }
#endif

        return buffer;
    }
};