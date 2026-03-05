# Migration Skill — MeOS Legacy → Modern C++17/CMake

This skill contains accumulated knowledge, patterns, gotchas, and useful scripts for migrating MeOS from its legacy Win32/MSBuild codebase (`code/`) to the modern modular CMake/vcpkg structure (`src/`).

## Iterative Migration Context

**This migration is run from scratch repeatedly.** Each attempt is executed by Ralph, analyzed, and discarded. Only the learnings persist (in this skill, progress.txt, and AGENTS.md). The PRD, skills, prompt.md, and ralph.sh are continuously improved based on each run's results.

**This is a fork of [melinsoftware/meos](https://github.com/melinsoftware/meos).** Upstream pushes changes at any time. Before each migration run, we sync with upstream. Therefore:

- **Never hardcode assumptions about legacy code** (line numbers, exact signatures, specific file contents).
- **Always read and parse legacy code dynamically** — enumerate files, grep for patterns, follow naming conventions.
- **When documenting patterns below, describe the *shape* of the code** (e.g., "domain files include gdioutput.h"), not exact locations that may shift.
- **If a pattern or gotcha becomes outdated after an upstream sync, update this file.**

## Architecture Overview

### Source Layout

| Legacy | Modern | Purpose |
|--------|--------|---------|
| `code/` (flat) | `src/app/` | Application entry point |
| `code/` | `src/domain/` | Core domain entities (`oRunner`, `oClass`, etc.) |
| `code/` | `src/net/` | Networking, REST API |
| `code/` | `src/db/` | Database (SQLite) |
| `code/` | `src/util/` | Utilities, parsers, cross-platform helpers |
| `code/` | `src/io/` | File I/O, import/export |

Each module is a **static library**. Bare `#include` works within modules via `target_include_directories(... PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})`.

### Domain Model

`oEvent` is the aggregate root — it owns all domain objects. Entity classes use the `o` prefix:

```
oBase (abstract: ID, change tracking)
├── oRunner  ─┐
├── oTeam     ├─ both extend oAbstractRunner
├── oClass
├── oClub
├── oCourse
├── oControl
├── oCard
├── oFreePunch
└── oPunch
```

## Critical Migration Patterns

### 1. Win32 Type Shimming

Redefine common Win32 types in `src/util/win_types.h` for Linux compatibility:
- `SYSTEMTIME`, `DWORD`, `HWND`, `RECT`, `COLOR`
- File search: `WIN32_FIND_DATA`
- String safe functions: `swprintf_s`, `snprintf_s`
- Basic GUI types for stubs

### 2. Header & Include Fixes

- **Case sensitivity**: Windows is case-insensitive; Linux is not. Filenames like `StdAfx.h` vs `stdafx.h` break on Linux. All known case mismatches in `code/` (147 total) have been fixed.

- **Verification**: Run `python3 verify_includes.py` from the project root to confirm zero case mismatches for `#include "..."` directives.

### 3. String Conversions

- Wide strings (`wstring`) are primary (Swedish/internationalized UI).
- Narrow `string` for internal/config data.
- Use `narrow()` and `widen()` from `meos_util.h` for conversions.
- `string2Wide()` / `wide2String()` support UTF-8 via `codecvt`.

Replace Win32-specific:
| Win32 | Standard C++ replacement |
|-------|--------------------------|
| `_itow_s` | `std::to_wstring` |
| `sprintf_s` | `snprintf` |
| `swprintf_s` | `swprintf` (shimmed) |
| `_stricmp` | `strcasecmp` (POSIX) |
| `_wcsicmp` | `wcscasecmp` (POSIX) |
| `MultiByteToWideChar` | `codecvt` / `widen()` |

### 4. Circular Dependency Management

- Move shared enums to `domain_header.h` or `src/util/common_enums.h`.
- Use forward declarations heavily.
- Template implementation files (`*impl.hpp`) must be included at the end of their headers.
- `SpecialPunch` enum was moved to `domain_header.h` to break circular deps.

### 5. Heavy Stubbing Strategy

The codebase is **extremely coupled**. Migrating one class often requires stubbing many others:

- Stub GUI classes: `gdioutput`, `oSpeaker`, `Table`
- Stub unmigrated domain entities in `src/domain/` (minimal `.h` with just enough API)
- `oEvent.h` needs extensive stubbing — it touches everything
- `oRunner` is ~230KB; use simplified implementation initially

### 5b. GUI Coupling in Domain Files (fix before migration)

9 of 11 domain files include `gdioutput.h` directly. Some only need it for non-GUI utility functions:

**Domain files including GUI headers:**

| File | gdioutput.h | Table.h | Tab*.h |
|---|---|---|---|
| oRunner.cpp | ✅ | ✅ | — |
| oEvent.cpp | ✅ | ✅ | TabBase, TabAuto, TabSI, TabList |
| oClass.cpp | ✅ | ✅ | — |
| oControl.cpp | ✅ | ✅ | — |
| oCard.cpp | ✅ | ✅ | — |
| oClub.cpp | ✅ | ✅ | — |
| oCourse.cpp | ✅ | ✅ | — |
| oTeam.cpp | ✅ | ✅ | — |
| oFreePunch.cpp | ✅ | ✅ | — |
| oBase.cpp | — | — | — |
| oPunch.cpp | — | — | — |

**Non-GUI utility functions living in gdioutput (extract to meos_util before migration):**
- `gdioutput::widen()` — used in oEvent.cpp, oEventResult.cpp
- `gdioutput::narrow()` — used in oRunner.cpp
- `gdioutput::toUTF8()` — used in oEvent.cpp
- `gdioutput::fromUTF8()` — used in oEvent.cpp

**Win32-specific functions in domain files (~58 calls):**
- `oDataContainer.cpp`: 38 uses (`swprintf_s`, `_itow_s`)
- `oClass.cpp`: 24 uses (mixed string operations)
- `oEventSQL.cpp`: 13 uses (`swprintf_s`)
- `oRunner.cpp`: 10 uses (`_wtoi`)
- `oEvent.cpp`: 7 uses (`_wtoi`)

**Win32 types in domain code (~90 uses):**
- `DWORD` (~50 uses) — replace with `uint32_t`
- `BOOL` (~30 uses) — replace with `bool`

**Path separators:** ~47 hardcoded backslash (`\\`) in file path strings (meos.cpp, zip.cpp, oClub.cpp, oEvent.cpp).

**oEvent → Tab* direct coupling (3 calls to remove before migration):**

| Line (approx) | Call | Purpose | Replacement strategy |
|---|---|---|---|
| ~7670 | `TabList::baseButtons(gdi, 1, false)` | Renders UI buttons in list view | Callback `std::function<void(gdioutput&, int, bool)>` registered by TabList |
| ~2700 | `TabAuto::tabAutoKillMachines()` | Kills automatic timing machines | Callback `std::function<void()>` registered by TabAuto |
| ~5200 | `TabSI::getSI(gdiBase()).setSubSecondMode(use)` | Sets SportIdent sub-second mode | Callback `std::function<void(bool)>` registered by TabSI |

oEvent.h should expose registration methods (e.g., `setOnKillMachines(std::function<void()>)`) and Tab classes register during init.

### 6. Redundant Overload Avoidance

On 64-bit Linux, `unsigned long` == `uint64_t`. Avoid redundant overloads for:
- `itos()` / `itow()` — don't provide both `unsigned long` and `uint64_t` versions

### 7. Data Container Initialization

For domain object unit tests, ensure `oe->oControlData` (or relevant data container) is initialized before testing. The `oDataContainer` system requires explicit setup.

### 8. Database Migration

- Always call `migrate()` after opening the database.
- Use `IF NOT EXISTS` for all migration SQL.
- Schema versioning via `_migrations` table.

## Useful One-Liners & Scripts

### Find all Win32 API calls in migrated code
```bash
grep -rn 'GetTickCount\|GetSystemTime\|CreateFile\|ReadFile\|WriteFile\|LoadResource\|FindResource\|MessageBox\|SetWindowText' src/
```

### Find remaining stdafx.h includes
```bash
grep -rni 'stdafx' src/
```

### Check for Windows-only headers
```bash
grep -rn '#include <windows.h>\|#include <tchar.h>\|#include <atlbase.h>\|#include <commctrl.h>' src/
```

### Find case-sensitivity issues in includes
```bash
for f in $(grep -roh '#include "[^"]*"' src/ | sed 's/#include "//;s/"//' | sort -u); do
  found=$(find src/ -iname "$(basename "$f")" 2>/dev/null | head -1)
  if [ -z "$found" ]; then
    echo "NOT FOUND: $f"
  fi
done
```

### List all migrated files vs legacy
```bash
echo "=== Migrated ==="
find src/ -name '*.cpp' -o -name '*.h' | wc -l
echo "=== Legacy ==="
find code/ -name '*.cpp' -o -name '*.h' | wc -l
```

### Compare class presence (legacy vs modern)
```bash
# Find domain classes in legacy that haven't been migrated
for cls in oRunner oTeam oClass oClub oCourse oControl oCard oFreePunch oPunch oEvent oBase; do
  legacy=$(find code/ -name "${cls}.*" 2>/dev/null | wc -l)
  modern=$(find src/ -name "${cls}.*" 2>/dev/null | wc -l)
  echo "$cls: legacy=$legacy modern=$modern"
done
```

### Build and test quick cycle
```bash
cmake --build --preset default 2>&1 | tail -20 && ctest --preset default
```

### Find compilation errors pattern
```bash
cmake --build --preset default 2>&1 | grep -E 'error:' | sed 's/.*error://' | sort | uniq -c | sort -rn | head -20
```

## Migration Status

### Migrated to `src/`
- **util**: meos_util, meosexception, xmlparser, csvparser, TimeStamp, timeconstants, localizer, inthashmap, intkeymap, random, win_types, common_enums, SICard (stub)
- **domain**: oBase, oDataContainer, oControl, oPunch, oClub, oRunner, oClass, oCourse, oTeam, oCard, oFreePunch, oEvent (skeleton), RunnerDB, MeosSQL, generalresult, metalist, oListInfo, classconfiginfo, datadefiners, domain_header
- **db**: SQLiteDatabase (with migration system)

### Not Yet Migrated
- **net**: RestService, RestServer
- **io**: IOF XML, CSV export, PDF generation
- **hw**: SportIdent reader protocol
- **ui**: gdioutput, all Tab* classes, speaker system
- **print**: PDF/print subsystem (libharu)

## Known Gotchas

0. **Legacy code is not static.** This is a fork that syncs with upstream. File contents, line numbers, function signatures, and even file names may change between migration runs. Always discover code structure dynamically rather than hardcoding assumptions.
1. **CMake static libs need at least one source file** — use `*_dummy.cpp` placeholder.
2. **GTest integration**: Need `find_package(GTest CONFIG REQUIRED)` + link `GTest::gtest GTest::gtest_main`.
3. **`enable_testing()`** must be in top-level `CMakeLists.txt`.
4. **StringCache** needs constructor for proper initialization (caused SegFault without it).
5. **Localization** uses `#` as separator for substitutions. Placeholders `X, Y, Z, W` must be in both key and translation.
6. **Static members and globals** (like `lang`) need careful handling in modular builds.
7. **oEvent.cpp** was replaced with minimal skeleton — don't try full migration in one step.
8. **vcpkg toolchain**: `CMAKE_TOOLCHAIN_FILE` must point to `vcpkg/scripts/buildsystems/vcpkg.cmake`.
