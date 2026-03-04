# Database Module

This module handles persistence using SQLite.

## SQLiteDatabase

`db::SQLiteDatabase` provides a wrapper around `sqlite3`.

### Connection Management
- `open(dbPath)`: Opens or creates a database file.
- `close()`: Closes the database.
- `execute(sql)`: Executes a SQL command.

### Schema Migrations
The module includes a simple migration system using a `_migrations` table.
- `migrate()`: Runs all pending migrations.
- `migrateToV1()`: Initial schema for `clubs` and `runners`.

## Usage
Always call `migrate()` after opening the database to ensure the schema is up to date.

```cpp
db::SQLiteDatabase db;
db.open("meos.db");
db.migrate();
```

## Tables
- `clubs`: `id`, `name`, `short_name`, `country_id`
- `runners`: `id`, `first_name`, `last_name`, `club_id`, `card_number`
