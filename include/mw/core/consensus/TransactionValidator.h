#pragma once

#include <mw/core/models/tx/Transaction.h>

class TransactionValidator
{
public:
	void Validate(const Transaction& transaction) const;

private:
	void ValidateFeatures(const TxBody& transactionBody) const;
	void ValidateKernelSums(const Transaction& transaction) const;
};