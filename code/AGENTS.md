# Copilot Instructions for MeOS (Legacy Codebase)

MeOS (Much Easier Orienteering System) is a Windows desktop application for managing orienteering competitions. It is built with C++17, Win32/GDI, and MSBuild.

## Build

MSBuild with Visual Studio 2022 (MSVC v143). Open `MeOS.sln` in Visual Studio.

Platforms: Win32 (x86) and x64. Precompiled header: `StdAfx.h`.

## Architecture

All source files live in this flat directory. Headers are included by bare filename (e.g., `#include "oBase.h"`).

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
- **REST API:** `RestService`/`RestServer` using the restbed library (`restbed/`)
- **Hardware:** `SportIdent` for RF card reader protocol (Windows serial port APIs)
- **Results:** `GeneralResult` (strategy pattern for pluggable scoring algorithms), `metalist` for output formatting
- **PDF:** libharu (`libharu/`)
- **Localization:** `.lng` files (key-value format, Swedish primary)

### Vendored dependencies

Libraries are vendored directly in subdirectories: `restbed/`, `libharu/`, `minizip/`, `mysql/`, `png/`, `sound/`. Platform-specific DLLs and libs in `dll/`, `dll64/`, `dll_debug/`, `lib/`, `lib64/`, `lib_db/`, `lib64_db/`.

## Conventions

### Naming

- Domain classes: `o` prefix (`oRunner`, `oEvent`)
- Pointer typedefs: `p` prefix for mutable (`pRunner`), `c` prefix for const (`cRunner`)
- Temporary/computed member variables: `t` prefix (`tStatus`, `tComputedTime`)
- Methods: camelCase (`getId()`, `updateChanged()`)

### Includes & Casing

**Every `#include "..."` directive MUST match the exact filename casing on disk.** This is required for compatibility with Linux and other case-sensitive filesystems. All legacy code in this directory has been fixed to follow this convention. Run `python3 verify_includes.py` from the root directory to confirm.

### Strings

Wide strings (`wstring`) are the primary string type (Swedish/internationalized UI). Narrow `string` is used for internal/config data. Conversion via `string2Wide()` in `meos_util.h`.

### String Utilities

**Use global string utility functions from `meos_util.h` for conversions:**

- `widen(const string&)`: Windows-1252 to `wstring` (standard MeOS conversion)
- `narrow(const wstring&)`: `wstring` to `string` (simple truncation)
- `toUTF8(const wstring&)`: `wstring` to UTF-8 `string`
- `fromUTF8(const string&)`: UTF-8 `string` to `wstring`
- `recodeToWide(const string&)`: `defaultCodePage` to `wstring` (for external data)
- `recodeToNarrow(const wstring&)`: `wstring` to `defaultCodePage`

**Legacy `gdioutput` static methods still exist but are thin wrappers.** Avoid adding new dependencies on `gdioutput.h` if you only need string utilities.

### Portable String Functions

**Always use standard C++ or portable wrappers from `meos_util.h` instead of Win32-specific string functions:**

- Use `compareStringIgnoreCase(a, b)` instead of `_stricmp`, `_wcsicmp`, or `lstrcmpi`.
- Use `snprintf` and `swprintf` instead of `sprintf_s` and `swprintf_s`. Note that standard `swprintf` requires the buffer size as the second argument.
- Use `std::wcstol` or `std::stoi` instead of `_wtoi`.
- Use `std::wcstod` instead of `_wtof`.
- Use `std::wcstoll` instead of `_wtoi64`.
- Use `itow(int)` or `itos(int)` for simple integer to string conversions (returns `wstring`/`string`).

Legacy string functions have been replaced in all domain files to ensure cross-platform compatibility.

### Path Normalization

**Always use cross-platform path construction and standard C++ filesystem APIs instead of hardcoded backslashes and Win32-specific functions:**

- Use `std::filesystem::path` (aliased as `path` in `StdAfx.h`) for all path manipulations.
- Use `operator/` for joining paths: `path p = path(baseDir) / fileName;`.
- Replace Win32-specific path functions like `_wsplitpath_s` or `_splitpath_s` with standard `std::filesystem::path` methods: `p.stem().wstring()`, `p.extension().wstring()`, `p.filename().wstring()`, `p.parent_path().wstring()`.
- Use `std::filesystem::exists(path)` instead of `GetFileAttributes(path)` or `fileExists` wrappers that call it.
- Convert back to `wstring` or `string` as needed: `p.wstring()`, `p.string()`, or `p.c_str()` (which returns `const wchar_t*` on Windows).
- For hardcoded path literals in strings, prefer forward slashes: `L"./../Lists/"` instead of `L".\\..\\Lists\\"`. Forward slashes are portable and work on both Windows and Linux.
- When checking for path separators, check for both: `if (c == '/' || c == '\\')`.

Legacy path joining, splitting, and backslash literals have been normalized in domain files to ensure they work on case-sensitive, forward-slash-preferring filesystems like Linux.

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
