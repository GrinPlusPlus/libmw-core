#pragma once

#include <memory>

// Forward Declarations
class IHeader;
class IHeaderFactory;
class Deserializer;

class Context
{
public:
    using CPtr = std::shared_ptr<const Context>;

    static Context::CPtr Create();

    const IHeaderFactory& GetHeaderFactory() const noexcept { return *m_pHeaderFactory; }

private:
    Context(std::unique_ptr<IHeaderFactory>&& pHeaderFactory) : m_pHeaderFactory(std::move(pHeaderFactory)) { }

    std::unique_ptr<IHeaderFactory> m_pHeaderFactory;
};

class IHeaderFactory
{
public:
    virtual ~IHeaderFactory() = default;

    virtual std::shared_ptr<const IHeader> Deserialize(const Context::CPtr&, Deserializer& deserializer) const = 0;
};