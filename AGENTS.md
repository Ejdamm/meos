# Copilot Instructions for MeOS

MeOS (Much Easier Orienteering System) is a Windows desktop application for managing orienteering competitions. A platform modernization is planned — see the PRD in `plan/prd-platform-modernization.md` for the target architecture.

## Project Overview

### Directory Structure

| Directory | Purpose |
|-----------|---------|
| `code/` | Legacy Windows-only codebase (MSBuild, Win32/GDI, MySQL). Has its own `AGENTS.md` with detailed architecture docs. |
| `src/` | Modern cross-platform codebase (CMake, C++20, React/TypeScript, SQLite). |
| `plan/` | PRD and planning artifacts for the modernization effort. |

## Legacy Codebase (`code/`)

See `code/AGENTS.md` for full details. In summary:

- ~190 source files in a flat directory
- C++17, Win32/GDI UI, MySQL backend
- MSBuild with Visual Studio 2022 (`MeOS.sln`)
- Vendored dependencies (restbed, libharu, minizip, mysql, png)

### Domain Model

`oEvent` is the aggregate root owning all domain objects:

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

### Other patterns

- `#pragma once` for header guards
- Heavy use of forward declarations to minimize include dependencies
- Smart pointers for ownership; raw pointers for parent/back-references
- No namespaces — flat namespace with `using std::` in `StdAfx.h`

## Testing Patterns

### C++ Backend

- **Google Test:** The primary framework for C++ unit and integration testing.
- **`meos_add_test`:** A CMake macro in `tests/CMakeLists.txt` for standardized test registration. It links against `GTest::gtest_main`.
- **Code Coverage:** Enable with `MEOS_ENABLE_COVERAGE=ON` (GCC/Clang only). It applies `--coverage` to both compilation and linking.
- **CTest:** Use `ctest` from the build directory to run tests.

### React Frontend

- **Vitest:** The primary framework for unit and component testing.
- **jsdom:** Used as the test environment for React components.
- **@testing-library/react:** Used for testing React components.
- **Coverage:** Configured with the `v8` provider in Vitest.

## CI/CD Pipeline

- **GitHub Actions:** Automated workflows for C++ and frontend.
- **`cpp.yml`:** Matrix strategy for Linux (ubuntu-latest) and Windows (windows-latest). Uses `lukka/run-vcpkg` for dependency management and `MEOS_ENABLE_CLANG_TIDY` for static analysis on Linux.
- **`frontend.yml`:** Node.js-based workflow for linting, testing, and building the React frontend.
- **`package-lock.json`:** Must be committed and tracked to ensure reliable `npm ci` runs in CI.
- **Static Analysis:** `.clang-tidy` and `.clang-format` are used for C++ code quality and formatting.

## Modernization

The PRD at `plan/prd-platform-modernization.md` describes the planned migration from Win32/GDI + MSBuild + MySQL to CMake + React/TypeScript + SQLite. The modernization has started, and a modern codebase exists in `src/`.

### Iterative Migration Approach

The migration is **run from scratch repeatedly** by Ralph (an autonomous agent loop in `plan/ralph.sh`). Each full attempt is analyzed, the PRD/skills/prompts are improved, and the migration is run again. The generated code is disposable — only the learnings persist across runs.

**This is a fork of [melinsoftware/meos](https://github.com/melinsoftware/meos).** We sync with upstream before each migration run. The legacy code in `code/` is therefore **not static** — do not make assumptions about exact file contents, line numbers, or function signatures. Always read and discover code structure dynamically.

**Key files for the migration loop:**

| File | Purpose |
|------|---------|
| `plan/prd-platform-modernization.md` | What to build (updated between runs) |
| `plan/prd.json` | Machine-readable PRD for Ralph (regenerated from the PRD) |
| `plan/prompt.md` | Instructions for each Ralph iteration (updated between runs) |
| `plan/ralph.sh` | The agent loop runner (updated between runs) |
| `plan/progress.txt` | Learnings from the current run (discarded between runs, patterns extracted first) |
| `.gemini/skills/migration/Skill.md` | Accumulated migration knowledge (persists across runs) |
