#pragma once

#pragma warning(push)
#pragma warning(disable:4244)
#pragma warning(disable:4267)
#pragma warning(disable:4334)
#pragma warning(disable:4018)
#include <mio/mmap.hpp>
#pragma warning(pop)

#include <mw/core/file/FilePath.h>
#include <mw/core/file/FileHelper.h>

class MemMap
{
public:
    MemMap(const FilePath& path) : m_path(path), m_mapped(false) { }
    MemMap(FilePath&& path) : m_path(std::move(path)), m_mapped(false) { }

    void Map()
    {
        if (m_mapped)
        {
            ThrowFile_F("{} already memory mapped", m_path);
        }

        if (FileHelper(m_path).GetFileSize() > 0)
        {
            std::error_code error;
            m_mmap = mio::make_mmap_source(m_path.ToPath(), error);
            if (error.value() > 0)
            {
                ThrowFile_F("Failed to mmap file: ({}) {}", error.value(), error.message());
            }

            m_mapped = true;
        }
    }

    void Unmap()
    {
        if (m_mapped)
        {
            m_mmap.unmap();
            m_mapped = false;
        }
    }

    std::vector<uint8_t> Read(const size_t position, const size_t numBytes) const
    {
        // TODO: Confirm m_mmap is mapped
        return std::vector<uint8_t>(m_mmap.cbegin() + position, m_mmap.cbegin() + position + numBytes);
    }

private:
    FilePath m_path;
    mio::mmap_source m_mmap;
    bool m_mapped;
};