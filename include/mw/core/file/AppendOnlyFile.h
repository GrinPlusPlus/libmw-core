#pragma once

#include <mw/core/file/FilePath.h>
#include <mw/core/file/FileHelper.h>
#include <mw/core/file/MemMap.h>
#include <mw/core/common/Logger.h>
#include <mw/core/traits/Batchable.h>

class AppendOnlyFile : public Traits::IBatchable
{
public:
    using Ptr = std::shared_ptr<AppendOnlyFile>;

    virtual ~AppendOnlyFile() = default;

    static AppendOnlyFile::Ptr Load(const FilePath& path)
    {
        FileHelper(path).CreateFileIfMissing();
        const size_t fileSize = FileHelper(path).GetFileSize();

        return std::make_shared<AppendOnlyFile>(path, fileSize);
    }

    virtual void Commit() override final
    {
        if (m_fileSize == m_bufferIndex && m_buffer.empty())
        {
            return;
        }

        if (m_fileSize < m_bufferIndex)
        {
            ThrowFile_F("Buffer index is past the end of {}", m_path);
        }

        m_mmap.Unmap();

        if (m_fileSize > m_bufferIndex)
        {
            FileHelper(m_path).Truncate(m_bufferIndex);
        }

        if (!m_buffer.empty())
        {
            std::ofstream file(m_path.c_str(), std::ios::out | std::ios::binary | std::ios::app);
            if (!file.is_open())
            {
                ThrowFile_F("Failed to open {} for writing", m_path);
            }

            file.seekp(m_bufferIndex, std::ios::beg);

            if (!m_buffer.empty())
            {
                file.write((const char*)& m_buffer[0], m_buffer.size());
            }

            file.close();
        }

        m_fileSize = m_bufferIndex + m_buffer.size();

        m_bufferIndex = m_fileSize;
        m_buffer.clear();

        m_mmap.Map();

        return true;
    }

    virtual void Rollback() override final
    {
        m_bufferIndex = m_fileSize;
        m_buffer.clear();
    }

    void Append(const std::vector<uint8_t>& data)
    {
        m_buffer.insert(m_buffer.end(), data.cbegin(), data.cend());
    }

    void Rewind(const uint64_t nextPosition)
    {
        // TODO: Should we commit first? Fail if already rewound without committing?

        if (nextPosition > (m_bufferIndex + m_buffer.size()))
        {
            ThrowFile_F("Tried to rewind past end of {}", m_path);
        }

        if (nextPosition <= m_bufferIndex)
        {
            m_buffer.clear();
            m_bufferIndex = nextPosition;
        }
        else
        {
            m_buffer.erase(m_buffer.begin() + nextPosition - m_bufferIndex, m_buffer.end());
        }
    }

    uint64_t GetSize() const noexcept
    {
        return m_bufferIndex + m_buffer.size();
    }

    std::vector<uint8_t> Read(const uint64_t position, const uint64_t numBytes) const
    {
        if ((position + numBytes) > (m_bufferIndex + m_buffer.size()))
        {
            ThrowFile_F("Tried to read past end of {}", m_path);
        }

        if (position < m_bufferIndex)
        {
            // TODO: Read from mapped and then from buffer, if necessary
            return m_mmap.Read(position, numBytes);
        }
        else
        {
            auto begin = m_buffer.cbegin() + position - m_bufferIndex;
            auto end = begin + numBytes;
            return std::vector<uint8_t>(begin, end);
        }
    }

private:
    AppendOnlyFile(const FilePath& path, const size_t fileSize)
        : m_path(path),
        m_mmap(path),
        m_fileSize(fileSize),
        m_bufferIndex(fileSize)
    {
        m_mmap.Map();
    }

    FilePath m_path;
    MemMap m_mmap;
    uint64_t m_fileSize;

    uint64_t m_bufferIndex;
    std::vector<uint8_t> m_buffer;
};