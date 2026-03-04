#include "sqlitedatabase.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

namespace db {

SQLiteDatabase::SQLiteDatabase() : db_(nullptr) {}

SQLiteDatabase::~SQLiteDatabase() {
    close();
}

void SQLiteDatabase::open(const std::string& dbPath) {
    if (db_) {
        close();
    }
    
    int rc = sqlite3_open(dbPath.c_str(), &db_);
    if (rc != SQLITE_OK) {
        std::string errMsg = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        db_ = nullptr;
        throw meosException(L"Cannot open database: " + std::wstring(errMsg.begin(), errMsg.end()));
    }
    dbPath_ = dbPath;
}

void SQLiteDatabase::close() {
    if (db_) {
        sqlite3_close(db_);
        db_ = nullptr;
    }
}

void SQLiteDatabase::execute(const std::string& sql) {
    if (!db_) {
        throw meosException(L"Database not open");
    }

    char* errMsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::string err(errMsg);
        sqlite3_free(errMsg);
        throw meosException(L"SQL execution error: " + std::wstring(err.begin(), err.end()));
    }
}

void SQLiteDatabase::createMigrationTable() {
    execute("CREATE TABLE IF NOT EXISTS _migrations (version INTEGER PRIMARY KEY);");
}

int SQLiteDatabase::getCurrentVersion() {
    const char* sql = "SELECT version FROM _migrations ORDER BY version DESC LIMIT 1;";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        return 0; // Possibly table doesn't exist yet
    }

    int version = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        version = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return version;
}

void SQLiteDatabase::setVersion(int version) {
    std::stringstream ss;
    ss << "INSERT INTO _migrations (version) VALUES (" << version << ");";
    execute(ss.str());
}

void SQLiteDatabase::migrate() {
    createMigrationTable();
    int currentVersion = getCurrentVersion();

    if (currentVersion < 1) {
        migrateToV1();
        setVersion(1);
    }
}

void SQLiteDatabase::migrateToV1() {
    // Initial schema for runners and clubs
    
    // Clubs table
    execute("CREATE TABLE IF NOT EXISTS clubs ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "name TEXT NOT NULL,"
            "short_name TEXT,"
            "country_id INTEGER"
            ");");

    // Runners table
    execute("CREATE TABLE IF NOT EXISTS runners ("
            "id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "first_name TEXT NOT NULL,"
            "last_name TEXT NOT NULL,"
            "club_id INTEGER,"
            "card_number TEXT,"
            "FOREIGN KEY (club_id) REFERENCES clubs(id)"
            ");");
}

} // namespace db
