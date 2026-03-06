# Copilot Instructions for MeOS

MeOS (Much Easier Orienteering System) is a Windows desktop application for managing orienteering competitions. A platform modernization is planned — see the PRD in `plan/prd-platform-modernization.md` for the target architecture.

## Project Overview

### Directory Structure

| Directory | Purpose |
|-----------|---------|
| `src/` | Modular source code organized by layer (app, domain, db, net, io, util, ui). See `src/AGENTS.md` for architecture details. |
| `legacy/` | Legacy build files and original architecture documentation (`legacy/AGENTS.md`). |
| `plan/` | PRD and planning artifacts for the modernization effort. |
| `thirdparty/` | Vendored dependencies and platform-specific binaries. |

## Source Code (`src/`)

The codebase has been reorganized into a modular layout in the `src/` directory.

- **App:** `src/app/` (entry points, resources)
- **Domain:** `src/domain/` (core entities)
- **DB:** `src/db/` (persistence)
- **Net:** `src/net/` (network/REST)
- **IO:** `src/io/` (file formats, hardware)
- **Util:** `src/util/` (utilities, stubs)
- **UI:** `src/ui/` (legacy Win32/GDI code)

See [src/AGENTS.md](src/AGENTS.md) for detailed conventions and architectural guidelines.

## Conventions

### Naming

- Domain classes: `o` prefix (`oRunner`, `oEvent`)
- Pointer typedefs: `p` prefix for mutable (`pRunner`), `c` prefix for const (`cRunner`)
- Temporary/computed member variables: `t` prefix (`tStatus`, `tComputedTime`)
- Methods: camelCase (`getId()`, `updateChanged()`)

### Includes & Casing

**Every `#include "..."` directive MUST match the exact filename casing on disk.** This is required for compatibility with Linux and other case-sensitive filesystems. All code in `src/` follows this convention.

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

### Portable String Functions

**Always use standard C++ or portable wrappers from `meos_util.h` instead of Win32-specific string functions:**

- Use `compareStringIgnoreCase(a, b)` instead of `_stricmp`, `_wcsicmp`, or `lstrcmpi`.
- Use `snprintf` and `swprintf` instead of `sprintf_s` and `swprintf_s`. Note that standard `swprintf` requires the buffer size as the second argument.
- Use `std::wcstol` or `std::stoi` instead of `_wtoi`.
- Use `std::wcstod` instead of `_wtof`.
- Use `std::wcstoll` instead of `_wtoi64`.
- Use `itow(int)` or `itos(int)` for simple integer to string conversions (returns `wstring`/`string`).

### Error handling

Custom exception `meosException` (with `wwhat()` for wide-string messages) and `meosCancel` for cancellation. Most functions prefer returning bool/error codes; exceptions are for critical failures.

### Other patterns

- `#pragma once` for header guards
- Heavy use of forward declarations to minimize include dependencies
- Smart pointers for ownership; raw pointers for parent/back-references
- No namespaces — flat namespace with `using std::` in `StdAfx.h`

## Modernization

The PRD at `plan/prd-platform-modernization.md` describes the planned migration from Win32/GDI + MSBuild + MySQL to CMake + React/TypeScript + SQLite.

**The CMake build system (`CMakeLists.txt` in project root) supports building modules as static libraries on Linux using vcpkg.**

### Iterative Migration Approach

The migration is **run from scratch repeatedly** by Ralph (an autonomous agent loop in `plan/ralph.sh`). Each full attempt is analyzed, the PRD/skills/prompts are improved, and the migration is run again. The generated code is disposable — only the learnings persist across runs.

**This is a fork of [melinsoftware/meos](https://github.com/melinsoftware/meos).** We sync with upstream before each migration run. The source code in `src/` is therefore **not static** — do not make assumptions about exact file contents, line numbers, or function signatures. Always read and discover code structure dynamically.

**Key files for the migration loop:**

| File | Purpose |
|------|---------|
| `plan/prd-platform-modernization.md` | What to build (updated between runs) |
| `plan/prd.json` | Machine-readable PRD for Ralph (regenerated from the PRD) |
| `plan/prompt.md` | Instructions for each Ralph iteration (updated between runs) |
| `plan/ralph.sh` | The agent loop runner (updated between runs) |
| `plan/progress.txt` | Learnings from the current run (discarded between runs, patterns extracted first) |
| `.gemini/skills/migration/Skill.md` | Accumulated migration knowledge (persists across runs) |
