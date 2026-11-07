# KalyaDB

**A new database engine for the Kalya ecosystem.**

KalyaDB is an independent database-engine project being designed from the ground up. The goal is to give Kalya applications a storage system that is understandable, dependable, modular, and designed to work locally.

> **Project status: Early planning.** The repository currently contains project documentation and the initial development website. The database engine is not implemented yet, and no production-readiness or compatibility claims are being made.

## Project goals

- Build a database engine rather than depend on an existing database as the core product.
- Keep the architecture modular so storage, querying, schemas, indexes, and transactions can evolve independently.
- Prioritise data integrity, predictable behaviour, and clear error handling.
- Support local-first use inside the Kalya desktop application.
- Document design decisions before they become difficult to change.

## Repository layout

| Path | Purpose |
| --- | --- |
| `engine/` | Database engine implementation (not yet started) |
| `docs/` | GitHub Pages development site and technical documentation |
| `docs/architecture.md` | Proposed engine architecture |
| `docs/roadmap.md` | Development milestones |
| `.gitignore` | Ignores generated files, local data, secrets, and build output |

## Development principles

1. **Data safety first.** Persistence and recovery must be designed and tested before the engine is trusted with important data.
2. **Explicit behaviour.** Document file formats, query semantics, limits, and error conditions.
3. **Small, testable components.** Keep storage, parsing, execution, and metadata responsibilities separate.
4. **No pretend features.** A feature is not considered implemented until it has tests and documentation.
5. **Local-first by design.** The initial integration target is the Kalya desktop application.

## Documentation

- [Development website](https://kalya-org.github.io/KalyaDB/)
- [Architecture](docs/architecture.md)
- [Roadmap](docs/roadmap.md)
- [Engine status](engine/README.md)
- [Contributing](CONTRIBUTING.md)
- [Security policy](SECURITY.md)

## License

KalyaDB is released under the [MIT License](LICENSE).
