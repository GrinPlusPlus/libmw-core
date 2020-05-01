#pragma once

#include <mw/core/models/tx/Output.h>
#include <mw/core/mmr/LeafIndex.h>
#include <mw/core/traits/Serializable.h>
#include <mw/core/serialization/Serializer.h>

class UTXO : public Traits::ISerializable
{
public:
    using CPtr = std::shared_ptr<const UTXO>;

    UTXO(const uint64_t blockHeight, mmr::LeafIndex&& leafIndex, Output&& output)
        : m_blockHeight(blockHeight), m_leafIndex(std::move(leafIndex)), m_output(std::move(output)) { }

    uint64_t GetBlockHeight() const noexcept { return m_blockHeight; }
    const mmr::LeafIndex GetLeafIndex() const noexcept { return m_leafIndex; }
    const Output& GetOutput() const noexcept { return m_output; }

    Serializer& Serialize(Serializer& serializer) const noexcept
    {
        return serializer
            .Append<uint64_t>(m_blockHeight)
            .Append<uint64_t>(m_leafIndex.GetLeafIndex())
            .Append(m_output);
    }

    static UTXO::CPtr Deserialize(const Context::CPtr& pContext, Deserializer& deserializer)
    {
        // TODO: Implement
        throw std::exception();
    }

private:
    uint64_t m_blockHeight;
    mmr::LeafIndex m_leafIndex;
    Output m_output;
};