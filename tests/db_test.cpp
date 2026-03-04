#include <gtest/gtest.h>
#include "sqlitedatabase.h"
#include <filesystem>

class SQLiteDatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        dbPath = "test_meos.db";
        if (std::filesystem::exists(dbPath)) {
            std::filesystem::remove(dbPath);
        }
    }

    void TearDown() override {
        if (std::filesystem::exists(dbPath)) {
            std::filesystem::remove(dbPath);
        }
    }

    std::string dbPath;
};

TEST_F(SQLiteDatabaseTest, OpenAndClose) {
    db::SQLiteDatabase db;
    EXPECT_NO_THROW(db.open(dbPath));
    EXPECT_TRUE(std::filesystem::exists(dbPath));
    db.close();
}

TEST_F(SQLiteDatabaseTest, Migrate) {
    db::SQLiteDatabase db;
    db.open(dbPath);
    EXPECT_NO_THROW(db.migrate());
    
    // Check if tables exist
    EXPECT_NO_THROW(db.execute("SELECT * FROM clubs;"));
    EXPECT_NO_THROW(db.execute("SELECT * FROM runners;"));
    EXPECT_NO_THROW(db.execute("SELECT * FROM _migrations;"));
}

TEST_F(SQLiteDatabaseTest, MigrationTwice) {
    db::SQLiteDatabase db;
    db.open(dbPath);
    db.migrate();
    
    // Running migrate again should not fail
    EXPECT_NO_THROW(db.migrate());
}

TEST_F(SQLiteDatabaseTest, InsertAndSelect) {
    db::SQLiteDatabase db;
    db.open(dbPath);
    db.migrate();
    
    EXPECT_NO_THROW(db.execute("INSERT INTO clubs (name, short_name) VALUES ('OK Orion', 'OKO');"));
    EXPECT_NO_THROW(db.execute("INSERT INTO runners (first_name, last_name, club_id) VALUES ('Adam', 'Georgsson', 1);"));
}
