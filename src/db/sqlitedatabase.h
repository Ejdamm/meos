#pragma once

#include <string>
#include <vector>
#include <memory>
#include <sqlite3.h>
#include "meosexception.h"

namespace db {

class SQLiteDatabase {
public:
    SQLiteDatabase();
    ~SQLiteDatabase();

    void open(const std::string& dbPath);
    void close();

    void execute(const std::string& sql);
    
    // Simple schema migration system
    void migrate();

private:
    sqlite3* db_ = nullptr;
    std::string dbPath_;

    void createMigrationTable();
    int getCurrentVersion();
    void setVersion(int version);
    
    // Migration steps
    void migrateToV1(); // Initial schema
};

} // namespace db
