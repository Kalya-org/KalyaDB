# KalyaDB Engine

The engine implementation will live in this directory.

**Current status:** no database engine code has been implemented yet.

Before adding runtime code, the project will decide:

- implementation language and minimum runtime;
- public API and data model;
- on-disk format and versioning;
- query syntax and execution model;
- transaction, concurrency, and recovery guarantees;
- test and benchmark strategy.

Keep prototypes clearly labelled and separate from stable APIs. Never test recovery or corruption handling against valuable data.
