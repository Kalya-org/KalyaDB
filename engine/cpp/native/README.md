# KalyaDB Native Core (C++)

This directory is the planned home of the native database engine.

The first commit establishes a small CMake library and version API. It is
scaffolding only: there is no storage engine, query executor, transaction
manager, or persistent database format yet.

## Build

Requires CMake 3.20+ and a C++17 compiler.

```sh
cmake -S . -B build
cmake --build build
```

The native core will own database-critical behaviour. Other language layers
must call into this core rather than implement competing storage engines.
