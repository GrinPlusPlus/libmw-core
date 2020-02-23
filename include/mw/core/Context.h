#pragma once

#include <mw/core/serialization/Deserializer.h>
#include <mw/core/serialization/Json.h>

#include <memory>

// Forward Declarations
class IBlock;
class IBlockFactory;
class IHeader;
class IHeaderFactory;
class IKernel;
class IKernelFactory;
class Deserializer;

// TODO: This should be a singleton instead
class Context
{
public:
    using CPtr = std::shared_ptr<const Context>;

    static Context::CPtr Create();

    const IBlockFactory& GetBlockFactory() const noexcept { return *m_pBlockFactory; }
    const IHeaderFactory& GetHeaderFactory() const noexcept { return *m_pHeaderFactory; }
    const IKernelFactory& GetKernelFactory() const noexcept { return *m_pKernelFactory; }

private:
    Context(
        std::unique_ptr<IBlockFactory>&& pBlockFactory,
        std::unique_ptr<IHeaderFactory>&& pHeaderFactory,
        std::unique_ptr<IKernelFactory>&& pKernelFactory)
        : m_pBlockFactory(std::move(pBlockFactory)),
        m_pHeaderFactory(std::move(pHeaderFactory)),
        m_pKernelFactory(std::move(pKernelFactory)) { }

    std::unique_ptr<IBlockFactory> m_pBlockFactory;
    std::unique_ptr<IHeaderFactory> m_pHeaderFactory;
    std::unique_ptr<IKernelFactory> m_pKernelFactory;
};

class IBlockFactory
{
public:
    virtual ~IBlockFactory() = default;

    virtual std::shared_ptr<const IBlock> Deserialize(Deserializer& deserializer) const = 0;
    virtual std::shared_ptr<const IBlock> FromJSON(const Json& json) const = 0;
};

class IHeaderFactory
{
public:
    virtual ~IHeaderFactory() = default;

    virtual std::shared_ptr<const IHeader> Deserialize(Deserializer& deserializer) const = 0;
    virtual std::shared_ptr<const IHeader> FromJSON(const Json& json) const = 0;
};

class IKernelFactory
{
public:
    virtual ~IKernelFactory() = default;

    virtual std::shared_ptr<const IKernel> Deserialize(Deserializer& deserializer) const = 0;
    virtual std::shared_ptr<const IKernel> FromJSON(const Json& json) const = 0;
};