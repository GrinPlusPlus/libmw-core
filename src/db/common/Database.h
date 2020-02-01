#pragma once

#include "DBTable.h"
#include "DBTransaction.h"
#include "DBEntry.h"
#include "DBLogger.h"

#include <mw/core/Context.h>
#include <mw/core/file/FilePath.h>
#include <mw/core/traits/Batchable.h>
#include <mw/core/common/Lock.h>

#include <leveldb/db.h>
#include <leveldb/filter_policy.h>
#include <vector>
#include <cassert>
#include <memory>

class Database : public Traits::IBatchable
{
public:
    using Ptr = std::shared_ptr<Database>;

    static Database::Ptr Open(const Context::CPtr& pContext, const FilePath& path)
    {
        path.GetParent().CreateDirIfMissing();

        std::unique_ptr<DBLogger> pLogger = std::make_unique<DBLogger>();

        leveldb::Options options;
        options.create_if_missing = true;
        options.info_log = new DBLogger();
        options.filter_policy = leveldb::NewBloomFilterPolicy(10); // TODO: Determine optimal size

        // Snappy compression is fast, but not useful for pseudorandom data like hashes & commitments.
        options.compression = leveldb::kNoCompression;

        leveldb::DB* pDB = nullptr;
        leveldb::Status status = leveldb::DB::Open(options, path.u8string(), &pDB); // TODO: Add tests for windows unicode paths.
        if (!status.ok())
        {
            ThrowDatabase_F("Open failed with status {}", status.ToString());
        }

        return std::shared_ptr<Database>(new Database(pContext, std::move(options), pDB));
    }

    virtual ~Database()
    {
        delete m_pDB;
        delete m_options.filter_policy;
        delete m_options.info_log;
    }

    //
    // Operations
    //
    template<typename T,
        typename SFINAE = typename std::enable_if_t<std::is_base_of_v<Traits::ISerializable, T>>>
    std::unique_ptr<DBEntry<T>> Get(const DBTable& table, const std::string& key) const noexcept
    {
        if (m_pTx != nullptr)
        {
            return m_pTx->Get<T>(m_pContext, table, key);
        }

        std::string itemStr;
        leveldb::Status status = m_pDB->Get(leveldb::ReadOptions(), table.BuildKey(key), &itemStr);
        if (status.ok())
        {
            Deserializer deserializer(std::vector<uint8_t>(itemStr.cbegin(), itemStr.cend()));
            return std::make_unique<DBEntry<T>>(key, T::Deserialize(m_pContext, deserializer));
        }

        return nullptr;
    }

    template<typename T,
        typename SFINAE = typename std::enable_if_t<std::is_base_of_v<Traits::ISerializable, T>>>
    void Put(const DBTable& table, const std::vector<DBEntry<T>>& entries)
    {
        assert(!entries.empty());

        if (m_pTx != nullptr)
        {
            m_pTx->Put(table, entries);
        }
        else
        {
            DBTransaction(m_pDB).Put(table, entries).Commit();
        }
    }

    // FUTURE: std::vector<DBEntry<T>> CustomGet(const DBTable& table, const Query& query) const;

    //
    // Batchable
    //
    virtual void OnInitWrite() override final
    {
        assert(m_pTx == nullptr);
        m_pTx = std::make_shared<DBTransaction>(m_pDB);
    }

    virtual void OnEndWrite() override final
    {
        m_pTx.reset();
    }

    virtual void Commit() override final
    {
        assert(m_pTx != nullptr);
        m_pTx->Commit();
    }
    virtual void Rollback() override final
    {
        assert(m_pTx != nullptr);
        m_pTx.reset();
    }
    
private:
    Database(const Context::CPtr& pContext, leveldb::Options&& options, leveldb::DB* pDB) noexcept
        : m_pContext(pContext), m_options(std::move(options)), m_pDB(pDB), m_pTx(nullptr) { }

    Context::CPtr m_pContext;
    leveldb::Options m_options;
    leveldb::DB* m_pDB;
    DBTransaction::Ptr m_pTx;
};