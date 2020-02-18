#pragma once

#include <mw/core/file/FilePath.h>
#include <mw/core/traits/Printable.h>

class File : public Traits::IPrintable
{
public:
    File(const FilePath& path) : m_path(path) { }
    File(FilePath&& path) : m_path(std::move(path)) { }

    virtual ~File() = default;

    void Truncate(const uint64_t size);
    void Rename(const std::string& filename);
    std::vector<uint8_t> ReadBytes() const;
    size_t GetSize() const;

    //
    // Traits
    //
    virtual std::string Format() const override final { return m_path.ToPath().u8string(); }

private:
    FilePath m_path;
};