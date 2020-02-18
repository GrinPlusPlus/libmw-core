#include <mw/core/file/File.h>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <unistd.h>
#endif

void File::Truncate(const uint64_t size)
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

void File::Rename(const std::string& filename)
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

std::vector<uint8_t> File::ReadBytes() const
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

size_t File::GetSize() const
{
    std::error_code ec;
    const size_t size = (size_t)fs::file_size(m_path, ec);
    if (ec)
    {
        ThrowFile_F("Failed to determine size of {}", *this);
    }

    return size;
}