# KalyaDB Native Core (C++)

This directory contains the experimental native database engine.

## Current prototype

The native API can:

- Create a `.kdb` file with a format signature and format version.
- Append length-prefixed records and read them in insertion order.
- Create named tables, list them, insert table records, and read them in insertion order.
- Execute a small SQL subset:
  - `CREATE TABLE users;`
  - `INSERT INTO users VALUES ('Ada');`
  - `SELECT * FROM users;`
- Accept SQL keywords in either upper- or lowercase.
- Read single-quoted text values; doubled apostrophes represent an apostrophe (for example, `'O''Reilly'`).

The current SQL prototype stores one text value per row. It does not yet support column definitions, data types, multiple values per row, filtering, updates, deletes, joins, indexes, transactions, concurrency control, crash recovery, or encryption.

The current record limit is 1 MiB and table names are limited to 255 bytes. Table metadata and rows are currently stored as reserved records in the append-only log; this is an early prototype, not a final table/page format. The file format is experimental and may change before a stable release. Do not use it for important or sensitive data.

## Build and test

Requires CMake 3.20+ and a C++17 compiler.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The native core owns database-critical behaviour. Other language layers should call into this core rather than implement competing storage engines.
