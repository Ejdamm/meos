# MeOS Modular Source Code (`src/`)

This directory contains the modernized, modularized source code for MeOS.

## Module Structure

| Module | Directory | Purpose |
|--------|-----------|---------|
| **App** | `src/app/` | Application entry point, main loop, configuration, and resources. |
| **Domain** | `src/domain/` | Core domain entities (`oRunner`, `oEvent`, etc.) and business logic. |
| **Database** | `src/db/` | Persistence layer (originally MySQL, migrating to SQLite). |
| **Network** | `src/net/` | Network communication, REST API, and online results. |
| **IO** | `src/io/` | File format handling (XML, CSV, PDF), printing, and hardware interfaces (SportIdent). |
| **Util** | `src/util/` | Foundation utilities, string conversions, and platform shims. |
| **UI** | `src/ui/` | Legacy Win32/GDI UI code (to be replaced by React/TypeScript). |

## Build System

The project uses CMake with vcpkg. Each module is defined as a static library.

- `util` (no dependencies)
- `domain` (depends on `util`)
- `db`, `net`, `io` (depend on `domain`, `util`)
- `ui` (depends on `domain`, `util`, `io`)
- `meos` executable (depends on all modules)

## Conventions

### Includes

**Bare includes are supported.** The CMake configuration adds all module directories to the include path.
Use `#include "oBase.h"` instead of `#include "domain/oBase.h"`.

### Precompiled Header

`src/util/StdAfx.h` is the precompiled header used by all modules. It includes common standard library headers and platform shims.

### Path Normalization

Use `std::filesystem::path` (aliased as `path` in `StdAfx.h`) for all path manipulations. Forward slashes are preferred for portability.

### UI Decoupling

Domain classes MUST NOT depend on UI classes. Use `std::function` callbacks registered in `src/app/meos.cpp` to communicate from domain to UI.
