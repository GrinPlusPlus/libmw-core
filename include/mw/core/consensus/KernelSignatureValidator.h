#pragma once

#include <mw/core/common/Logger.h>
#include <mw/core/crypto/Crypto.h>
#include <mw/core/models/tx/IKernel.h>
#include <mw/core/exceptions/ValidationException.h>

class KernelSignatureValidator
{
public:
    // Verify the tx kernels.
    static void VerifyKernelSignatures(const std::vector<IKernel::CPtr>& kernels)
    {
        std::vector<const Commitment*> commitments;
        commitments.reserve(kernels.size());

        std::vector<const Signature*> signatures;
        signatures.reserve(kernels.size());

        std::vector<Hash> msgs;
        msgs.reserve(kernels.size());

        std::vector<const Hash*> messages;
        messages.reserve(kernels.size());

        // Verify the transaction proof validity. Entails handling the commitment as a public key and checking the signature verifies with the fee as message.
        for (size_t i = 0; i < kernels.size(); i++)
        {
            const IKernel::CPtr& pKernel = kernels[i];
            commitments.push_back(&pKernel->GetExcess());
            signatures.push_back(&pKernel->GetSignature());
            msgs.emplace_back(pKernel->GetSignatureMessage());
            messages.push_back(&msgs[i]);
        }

        if (!Crypto::VerifyKernelSignatures(signatures, commitments, messages))
        {
            ThrowValidation(EConsensusError::KERNEL_SIG);
        }
    }
};