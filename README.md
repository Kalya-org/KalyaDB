# KalyaDB

**An open-source relational database platform for the Kalya ecosystem.**

KalyaDB is being developed as more than a native database library. Its core will be an independent relational database engine, with service and developer-facing layers built around it so applications can connect through documented APIs.

> **Status:** Experimental development. The native C++ prototype currently supports a limited SQL subset and a browser playground. It is not production-ready and must not be used for important or sensitive data.

## Product direction

KalyaDB takes architectural inspiration from platforms such as Supabase, while developing its own engine and services.

- **Relational engine:** SQL, schemas, typed columns, constraints, relationships, indexes, and transactions.
- **Storage and recovery:** durable files, integrity checks, backups, and recovery.
- **Database service:** a managed local or self-hosted service that applications can connect to.
- **Developer APIs:** documented interfaces and client libraries for JavaScript/TypeScript and other languages.
- **Authentication and authorization:** identities, roles, and policies for controlling access.
- **Realtime:** subscriptions to committed database changes.
- **Dashboard and tools:** project management, table browsing, SQL editor, API settings, and database administration.

These are product goals, not claims that all features already exist. The relational engine is the first priority; service features depend on a reliable engine and a defined security model.

## Architecture principles

1. **One source of truth:** the relational engine owns data and transaction semantics. APIs and SDKs call the engine; they do not implement separate databases.
2. **Relational correctness first:** schemas, data types, primary keys, foreign keys, constraints, and transactions must be tested before higher-level services rely on them.
3. **Secure by default:** network access, authentication, authorization, and secrets handling must be designed before exposing a database service.
4. **Local-first, service-capable:** support embedded/local use and leave a clear boundary for a separately hosted service.
5. **No pretend features:** a feature is implemented only when code, tests, and documentation support it.

## Repository layout

| Path | Purpose |
| --- | --- |
| `engine/cpp/native/` | Experimental native database engine |
| `docs/` | GitHub Pages development site and technical documentation |
| `docs/architecture.md` | Target multi-layer architecture |
| `docs/roadmap.md` | Development milestones |
| `.github/workflows/` | Native and WebAssembly build workflows |

## Current prototype

The native engine and WebAssembly playground are experimental. The playground currently demonstrates a small SQL subset and saves its database in the current browser using IndexedDB. That browser storage is not a hosted database service, is not shared between devices, and can be cleared by the browser.

Try the [SQL Playground](https://kalya-org.github.io/KalyaDB/playground.html), read the [architecture](docs/architecture.md), or see the [roadmap](docs/roadmap.md).

## Development and safety

- Build and test the native engine with CMake and CTest.
- Keep database-format changes versioned and tested.
- Use disposable test databases for corruption and recovery tests.
- Do not put secrets or sensitive information in the browser playground.

## License

KalyaDB is released under the [MIT License](LICENSE).
