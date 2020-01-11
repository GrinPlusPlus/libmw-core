#pragma once

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include <mw/core/common/Logger.h>
#include <mw/core/file/FilePath.h>
#include <mw/core/traits/Printable.h>

class FileHelper
{
public:
    FileHelper(const FilePath& path) : m_path(path) { }
    FileHelper(FilePath&& path) : m_path(std::move(path)) { }

    void CreateFileIfMissing()
    {
        if (m_path.Exists())
        {
            if (m_path.IsDirectory())
            {
                ThrowFile_F("{} is a directory", m_path);
            }
        }

        LOG_INFO_F("File {} does not exist. Creating it now.", m_path);
        std::ofstream outFile(m_path.c_str(), std::ios::out | std::ios::binary | std::ios::app);
        if (!outFile.is_open())
        {
            ThrowFile_F("Failed to create file: {}", m_path);
        }

        outFile.close();
    }

    void Truncate(const uint64_t size)
    {
        bool success = false;

#if defined(WIN32)
        HANDLE hFile = CreateFile(
            m_path.c_str(),
            GENERIC_WRITE,
            FILE_SHARE_READ,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
        );

        LARGE_INTEGER li;
        li.QuadPart = size;
        success = SetFilePointerEx(hFile, li, NULL, FILE_BEGIN) && SetEndOfFile(hFile);

        CloseHandle(hFile);
#else
        sucess = (truncate(m_path.c_str(), size) == 0);
#endif

        if (!success)
        {
            ThrowFile_F("Failed to truncate {}", m_path);
        }
    }


    void Rename(const std::string& filename)
    {
        if (m_path.IsDirectory())
        {
            // FUTURE: Support renaming directories?
            ThrowFile("Rename not implemented for directories");
        }

        const FilePath parent(m_path.parent_path());
        if (parent == m_path)
        {
            ThrowFile_F("Can't find parent path for {}", *this);
        }

        const FilePath destination = parent.GetChild(filename);
        if (destination.Exists())
        {
            destination.Remove();
        }

        std::error_code ec;
        fs::rename(m_path, destination.m_path, ec);
        if (ec)
        {
            ThrowFile_F("Failed to rename {} to {}", *this, destination);
        }

        m_path = destination.m_path;
    }

    std::vector<uint8_t> ReadBytes() const
    {
        std::error_code ec;
        if (!fs::exists(m_path, ec) || ec)
        {
            ThrowFile_F("{} not found", *this);
        }

        std::ifstream file(m_path, std::ios::in | std::ios::binary);
        if (!file.is_open())
        {
            ThrowFile_F("Failed to open {} for reading", *this);
        }

        const size_t size = (size_t)fs::file_size(m_path, ec);

        std::vector<uint8_t> bytes((size_t)size);
        file.seekg(0, std::ios::beg);
        file.read((char*)bytes.data(), size);
        file.close();

        return bytes;
    }

    size_t GetFileSize() const
    {
        std::error_code ec;
        const size_t size = (size_t)fs::file_size(m_path, ec);
        if (ec)
        {
            ThrowFile_F("Failed to determine size of {}", *this);
        }

        return size;
    }

    //
    // Traits
    //
    virtual std::string Format() const override final { return m_path.ToPath().u8string(); }

private:
    FilePath m_path;
};