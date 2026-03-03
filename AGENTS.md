# Copilot Instructions for MeOS

MeOS (Much Easier Orienteering System) is a Windows desktop application for managing orienteering competitions. It is built with C++17, Win32/GDI, and MSBuild.

## Build

### Modern (CMake + vcpkg)

Recommended for cross-platform development (Linux/Windows).

```bash
# Configure with presets
cmake --preset default
# Build
cmake --build --preset default
```

Dependencies are managed via `vcpkg.json`. Use `CMakePresets.json` to configure the `CMAKE_TOOLCHAIN_FILE` path for your local `vcpkg` installation.

### Legacy (MSBuild)

MSBuild-only (Visual Studio 2022, MSVC v143). Used for the original Windows-only GUI and legacy code in `code/`.

Platforms: Win32 (x86) and x64. Precompiled header: `StdAfx.h`.

## Architecture

All source files live in `code/` (flat directory). Headers are included by bare filename (e.g., `#include "oBase.h"`).

### Domain model

`oEvent` is the aggregate root — it owns collections of all domain objects. Domain entity classes use the `o` prefix:

```
oBase (abstract base: ID, change tracking, data interface)
├── oRunner  ─┐
├── oTeam     ├─ both extend oAbstractRunner (shared result logic)
├── oClass
├── oClub
├── oCourse
├── oControl
├── oCard
├── oFreePunch
└── oPunch
```

Key relationships: `oRunner`/`oTeam` → `oCard` (punch records), `oTeam` → `oRunner` (members), `oClass` → `oCourse`.

### UI layer

Tab-based GUI built on `gdioutput` (custom Win32/GDI wrapper). Each feature area is a `TabBase` subclass (`TabRunner`, `TabClass`, `TabCompetition`, etc.) — one tab per domain entity plus specialized tabs for results, speaker, automation.

### Persistence & integration

- **Database:** `MeosSQL` (ORM-like layer) → `mysqlwrapper` → MySQL
- **REST API:** `RestService`/`RestServer` using the restbed library
- **Hardware:** `SportIdent` for RF card reader protocol
- **Results:** `GeneralResult` (strategy pattern for pluggable scoring algorithms), `metalist` for output formatting
- **PDF:** libharu (`libharu/`)
- **Localization:** `.lng` files (key-value format, Swedish primary)

## Conventions

### Naming

- Domain classes: `o` prefix (`oRunner`, `oEvent`)
- Pointer typedefs: `p` prefix for mutable (`pRunner`), `c` prefix for const (`cRunner`)
- Temporary/computed member variables: `t` prefix (`tStatus`, `tComputedTime`)
- Methods: camelCase (`getId()`, `updateChanged()`)

### Strings

Wide strings (`wstring`) are the primary string type (Swedish/internationalized UI). Narrow `string` is used for internal/config data. Conversion via `string2Wide()` in `meos_util.h`.

### Error handling

Custom exception `meosException` (with `wwhat()` for wide-string messages) and `meosCancel` for cancellation. Most functions prefer returning bool/error codes; exceptions are for critical failures.

### Data container pattern

`oDataContainer` provides metadata-driven field definitions (`oDataInfo`). Access via `oDataInterface` (mutable) / `oDataConstInterface` (read-only), which auto-track changes.

### Other patterns

- `#pragma once` for header guards
- Heavy use of forward declarations to minimize include dependencies
- Smart pointers (`shared_ptr`, `unique_ptr`) for ownership; raw pointers for parent/back-references
- No namespaces — flat namespace with `using std::` in `StdAfx.h`
- Disabled warnings: 4267, 4244, 4018 (integer conversion/truncation)
