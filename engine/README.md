# KalyaDB Engine

The native engine is the relational core of the KalyaDB platform. It owns SQL semantics, schemas, constraints, transactions, and the durable database format. Service APIs and client SDKs will call this engine rather than implement separate storage systems.

## Current status

The C++ engine is an experimental prototype. It has a basic file format, named-table records, a limited SQL parser, and a WebAssembly build used by the browser playground. It is not production-ready.

The current SQL prototype supports a very small subset: table creation without column definitions, inserting one text value per row, and selecting all values. It does not yet implement a complete relational schema or relational integrity.

## 0.2.0 relational target

- Named table schemas and multiple columns.
- Initial TEXT and INTEGER data types.
- Row values validated against their column definitions.
- Structured query results with column names and typed values.
- Versioned schema-aware persistence.
- Native and WebAssembly tests/examples.

## Longer-term engine responsibilities

- Primary/foreign keys and constraints.
- UPDATE, DELETE, filtering, ordering, and joins.
- Indexes and query planning.
- Transactions, concurrency, and crash recovery.
- Backup, restore, integrity checks, and format migrations.

## Platform boundary

The engine is not the complete KalyaDB product. A separate service layer will later expose safe APIs, authentication, authorization, and realtime events. The dashboard and SDKs will be clients of that service. Do not expose database files directly over a network.

Use disposable test databases only. Do not rely on this prototype for important or sensitive data.
