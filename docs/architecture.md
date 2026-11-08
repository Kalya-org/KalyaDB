# KalyaDB Architecture

**Status: target architecture.** The components below describe the intended product. Only the limited native engine and browser playground prototypes currently exist.

KalyaDB is intended to become a relational database platform inspired by the broad product shape of Supabase. It will use its own database engine and service components rather than presenting a C++ library as the entire product.

## System layers

```text
Applications
  ├── JavaScript / TypeScript SDK
  ├── Other client SDKs
  └── Direct SQL tools (trusted environments only)
             |
             v
     KalyaDB API / Service
       ├── REST / RPC endpoints
       ├── Authentication
       ├── Authorization and policies
       ├── Realtime subscriptions
       └── Project and connection management
             |
             v
      Database engine interface
             |
             v
       Relational engine (C++)
       ├── SQL parser and planner
       ├── Relational executor
       ├── Catalog and schema manager
       ├── Constraints and relationships
       ├── Index manager
       ├── Transaction manager
       └── Storage and recovery
             |
             v
        Database files
```

The dashboard is a separate client of the service. It should not bypass authentication or security policies to access database files directly.

## 1. Relational engine

The engine is the source of truth for relational data and transaction behaviour. Its first relational capabilities should include:

- tables with named columns;
- data types such as TEXT and INTEGER;
- rows with multiple values;
- primary keys and unique constraints;
- foreign keys and referential integrity;
- NULL handling and NOT NULL constraints;
- SQL operations such as CREATE, INSERT, SELECT, UPDATE, and DELETE;
- indexes and transactions as the storage design matures.

The first implementation milestone is deliberately smaller: typed columns and multi-value rows. More advanced constraints must not be advertised until implemented and tested.

## 2. Storage, transactions, and recovery

The storage layer owns the on-disk format. The transaction manager must define atomicity, consistency, isolation, and durability. Before a network service depends on the engine, the project needs crash-recovery tests, concurrent-access rules, integrity checks, backup, restore, and format migration procedures.

## 3. Database service and API

A service process will expose selected database operations to applications. It should provide documented HTTP APIs and a controlled SQL connection path where appropriate. The service must use parameterized operations, validate requests, enforce authorization, apply limits, and avoid exposing raw database files.

The service is not the database engine: it is a secure interface to the engine. Multiple SDKs and the dashboard should use the same service contracts.

## 4. Authentication and authorization

Authentication identifies users and service identities. Authorization decides what they can access. The design should include projects, roles, API credentials, row/table-level policies, secret rotation, and audit-friendly administration. These features must be designed before public network access is enabled.

## 5. Realtime

Realtime should publish committed changes only. Subscribers must be authorized against the same access rules as ordinary reads. Delivery guarantees, ordering, reconnect behaviour, and backpressure need explicit definitions.

## 6. Dashboard and developer experience

The dashboard should eventually support project setup, table and relationship browsing, SQL queries, API documentation, user and role management, logs, backups, and service configuration. It is a client of the service, not a substitute for the engine.

## Build order

1. Make the engine relational and test its data integrity.
2. Establish storage format versioning, transactions, and recovery.
3. Define the engine/service boundary and a safe local service.
4. Add authenticated APIs and SDKs.
5. Add authorization policies and realtime.
6. Build the dashboard and deployment/administration tools.

## Current implementation boundary

The native C++ engine is an experimental prototype. The browser playground compiles that engine to WebAssembly and stores its own database in browser IndexedDB. GitHub Pages only hosts static files; it does not run a shared KalyaDB server. The playground is not a production service and must not be used for sensitive data.
