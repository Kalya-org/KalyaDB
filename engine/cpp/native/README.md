# KalyaDB Native Core (C++)

This directory contains the first experimental native database components.

## Current prototype

The storage API can:

- Create a `.kdb` file with a format signature and format version.
- Append length-prefixed records.
- Reopen a file and read records in insertion order.
- Reject invalid headers, unsupported format versions, oversized records, and truncated records.

The current record limit is 1 MiB. The file format is experimental and may change before a stable release. This prototype does not yet provide tables, queries, indexes, transactions, concurrency control, crash recovery, or encryption. Do not use it for important or sensitive data.

## Build and test

Requires CMake 3.20+ and a C++17 compiler.

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The native core owns database-critical behaviour. Other language layers should call into this core rather than implement competing storage engines.
