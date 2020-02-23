#pragma once

#include <mw/core/models/tx/Input.h>
#include <mw/core/models/tx/Output.h>
#include <mw/core/exceptions/ValidationException.h>
#include <set>

class CutThroughVerifier
{
public:
    static void VerifyCutThrough(const std::vector<Input>& inputs, const std::vector<Output>& outputs)
    {
        // TODO: Is it safe to assume duplicates were removed?
        // Create set with output commitments
        std::set<Commitment> commitments;
        std::transform(
            outputs.cbegin(), outputs.cend(),
            std::inserter(commitments, commitments.end()),
            [](const Output& output) { return output.GetCommitment(); }
        );

        // Verify none of the input commitments are in the commitments set
        const bool invalid = std::any_of(
            inputs.cbegin(), inputs.cend(),
            [&commitments](const Input& input) { return commitments.find(input.GetCommitment()) != commitments.cend(); }
        );
        if (invalid)
        {
            ThrowValidation(EConsensusError::CUT_THROUGH);
        }
    }
};