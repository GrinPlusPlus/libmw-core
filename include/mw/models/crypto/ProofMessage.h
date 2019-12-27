#pragma once

// Copyright (c) 2018-2019 David Burkett
// Distributed under the MIT software license, see the accompanying
// file LICENSE or http://www.opensource.org/licenses/mit-license.php.

#include <mw/models/crypto/BulletProofType.h>
#include <mw/models/crypto/BigInteger.h>

class ProofMessage
{
public:
    ProofMessage(BigInt<20>&& bytes) : m_bytes(std::move(bytes)) { }

    static ProofMessage FromKeyIndices(const std::vector<uint32_t>& keyIndices, const EBulletProofType& bulletproofType)
    {
        Serializer serializer;
        for (const uint32_t keyIndex : keyIndices)
        {
            serializer.Append(keyIndex);
        }

        BigInt<20> paddedPath = BigInt<20>::ValueOf(0);
        if (bulletproofType == EBulletProofType::ENHANCED)
        {
            paddedPath[2] = 1;
            paddedPath[3] = (uint8_t)keyIndices.size();
        }

        for (size_t i = 0; i < serializer.size(); i++)
        {
            paddedPath[i + 4] = serializer[i];
        }

        return ProofMessage(std::move(paddedPath));
    }

    std::vector<uint32_t> ToKeyIndices(const EBulletProofType& bulletproofType) const
    {
        ByteBuffer byteBuffer(m_bytes.vec());

        size_t length = 3;
        if (bulletproofType == EBulletProofType::ENHANCED)
        {
            byteBuffer.ReadU8(); // RESERVED: Always 0
            byteBuffer.ReadU8(); // Wallet Type
            byteBuffer.ReadU8(); // Switch Commits - Always true for now.
            length = byteBuffer.ReadU8();
        }
        else
        {
            const uint32_t first4Bytes = byteBuffer.ReadU32();
            if (first4Bytes != 0)
            {
                throw DeserializationEx("Failed to deserialize proof message.");
            }
        }

        if (length == 0)
        {
            length = 3;
        }

        std::vector<uint32_t> keyIndices(length);
        for (size_t i = 0; i < length; i++)
        {
            keyIndices[i] = byteBuffer.ReadU32();
        }

        return keyIndices;
    }

    const BigInt<20>& GetBytes() const { return m_bytes; }
    const uint8_t* data() const { return m_bytes.data(); }

private:
    BigInt<20> m_bytes;
};