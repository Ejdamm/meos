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
| `code/` (flat) | `src/app/` | Application entry point, resources, lang |
| `code/` | `src/domain/` | Core domain entities (`oRunner`, `oClass`, etc.) |
| `code/` | `src/net/` | Networking, REST API |
| `code/` | `src/db/` | Database (SQLite/MySQL wrapper) |
| `code/` | `src/util/` | Utilities, stubs, platform shims |
| `code/` | `src/io/` | File I/O, import/export, printing |
| `code/` | `src/ui/` | Legacy Win32/GDI code |

Each module is a **static library**. Bare `#include` works within modules via common include paths in `CMakeLists.txt`.

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

**Gotchas:**
- **Incomplete Types**: Classes like `GDIImplFontEnum` and `GDIImplFontSet` must be defined (even if empty) rather than just forward-declared when used as value types in `std::vector` or `std::map` members on Linux/GCC.
- **Missing Typedefs**: Ensure `INT`, `LPSIZE`, `LPOPENFILENAME`, and `LPBROWSEINFO` are defined in `win_types.h`.
- **Macro Redefinitions**: Wrap Win32 resource IDs (like `IDD_SPLASH`) in `#ifndef` to avoid conflicts with `resource.h`.
- **Macro vs Inline**: Prefer inline functions for Win32 API shims (like `OffsetRect`) to avoid issues with `return` statements and expression evaluation.

### 2. Header & Include Fixes

- **Case sensitivity**: Windows is case-insensitive; Linux is not. Filenames like `StdAfx.h` vs `stdafx.h` break on Linux. Fix all case mismatches in `code/` before migrating (there are ~147 known mismatches).
- **Verification**: After fixing, verify zero case mismatches for `#include "..."` directives by scanning `code/` and comparing against actual filenames on disk.

### 3. String Conversions

- Wide strings (`wstring`) are primary (Swedish/internationalized UI).
- Narrow `string` for internal/config data.
- **Use global string utility functions from `meos_util.h` for conversions:**
  - `widen(const string&)`: Windows-1252 to `wstring` (standard MeOS conversion)
  - `narrow(const wstring&)`: `wstring` to `string` (simple truncation)
  - `toUTF8(const wstring&)`: `wstring` to UTF-8 `string`
  - `fromUTF8(const string&)`: UTF-8 `string` to `wstring`
  - `recodeToWide(const string&)`: `defaultCodePage` to `wstring` (for external data)
  - `recodeToNarrow(const wstring&)`: `wstring` to `defaultCodePage`
- These return references to strings in `StringCache` for efficiency/brevity.
- **Note**: In legacy code, these functions exist as static methods on `gdioutput`. Extract them to global functions in `meos_util.h` and replace all static/member calls across the legacy codebase. `gdioutput` methods become thin wrappers for backward compatibility.

### 4. Win32 API Replacement Table

| Win32 | Standard C++ replacement |
|-------|--------------------------|
| `_itow_s` | `swprintf(buf, size, L"%d", val)` |
| `sprintf_s` | `snprintf` |
| `swprintf_s` | `swprintf` |
| `_stricmp` / `_wcsicmp` | `compareStringIgnoreCase(a, b)` |
| `lstrcmpi` | `compareStringIgnoreCase(a, b)` |
| `_wtoi` | `(int)std::wcstol(str, nullptr, 10)` |
| `_wtof` | `std::wcstod(str, nullptr)` |
| `_wtoi64` | `(long long)std::wcstoll(str, nullptr, 10)` |
| `MultiByteToWideChar` | `codecvt` / `widen()` |
| `CharLowerBuff` | `towlower(wchar_t)` in a loop |
| `FindResource` / `LoadResource` | `std::ifstream` + predefined search paths |
| `_wsopen_s` / `_read` / `_write` | `std::ifstream` / `std::ofstream` (binary mode) |
| `OffsetRect` | `inline BOOL OffsetRect(LPRECT lprc, int dx, int dy)` |
| `_wsplitpath_s` / `_splitpath_s` | `std::filesystem::path` methods: `.stem()`, `.extension()`, `.filename()`, `.parent_path()` |
| `GetFileAttributes` | `std::filesystem::exists` |

Replace all Win32-specific function calls (`_wtoi`, `sprintf_s`, `swprintf_s`, `_itow_s`, `_wtoi64`, `_wtof`, `_stricmp`, `_wcsicmp`, `lstrcmpi`) with standard C++ equivalents in domain files (`o*.cpp/h`, `generalresult.cpp/h`, `metalist.cpp/h`, `datadefiners.h`).

Replace all Win32 types (`DWORD`, `BOOL`, `WORD`, `BYTE`, `LPWSTR`, `LPCWSTR`) with standard C++ types (`uint32_t`, `bool`, `uint16_t`, `uint8_t`, `wchar_t*`, `const wchar_t*`) in domain files. Add `#include <cstdint>` to headers using these types.

### 5. Path Separators

- **Use `std::filesystem::path`** for all path manipulations and construction. Alias as `path` in `StdAfx.h`.
- Replace `path += L"\\file.ext"` with `(std::filesystem::path(path) / L"file.ext").wstring()`.
- Replace `GetFileAttributes` with `std::filesystem::exists`.
- Use forward slashes (`/`) in hardcoded path string literals: `L"./../Lists/"`.
- Check for both separators where needed: `if (c == '/' || c == '\\')`.
- Normalize the ~47 hardcoded backslash (`\\`) path strings found in files like `meos.cpp`, `zip.cpp`, `oClub.cpp`, `oEvent.cpp`, `oEventSpeaker.cpp`.

### 6. Circular Dependency Management

- Move shared enums (e.g., `SpecialPunch`) to `domain_header.h` or `src/util/common_enums.h` to break circular deps between domain entities.
- Use forward declarations heavily in `domain_header.h` to reduce circular dependencies between `oBase`, `oDataContainer`, `oEvent`, and UI classes.
- Template implementation files (`*impl.hpp`) must be included at the end of their headers (e.g., `intkeymap.hpp` includes `intkeymapimpl.hpp`).

### 7. Heavy Stubbing Strategy

The codebase is **extremely coupled**. Migrating one class often requires stubbing many others:

- Stub GUI classes: `gdioutput`, `oSpeaker`, `Table`
- Stub unmigrated domain entities in `src/domain/` (minimal `.h` with just enough API)
- `oEvent.h` needs extensive stubbing — it touches everything
- `oRunner` is ~230KB; use simplified implementation initially
- **Foundation Stubs**: Provide minimal implementations for heavily used methods of `oEvent`, `gdioutput`, and `Table` in `domain_module.cpp` to allow the domain library and its tests to link without pulling in the entire UI/Server layer.

### 8. GUI Coupling in Domain Files

9 of 11 domain files include `gdioutput.h` directly. Some only need it for non-GUI utility functions:

| File | gdioutput.h | Table.h | Tab*.h |
|---|---|---|---|
| oRunner.cpp | yes | yes | — |
| oEvent.cpp | yes | yes | TabBase, TabAuto, TabSI, TabList |
| oClass.cpp | yes | yes | — |
| oControl.cpp | yes | yes | — |
| oCard.cpp | yes | yes | — |
| oClub.cpp | yes | yes | — |
| oCourse.cpp | yes | yes | — |
| oTeam.cpp | yes | yes | — |
| oFreePunch.cpp | yes | yes | — |
| oBase.cpp | — | — | — |
| oPunch.cpp | — | — | — |

**Strategy**: Extract non-GUI utility functions (string conversions) from `gdioutput` to `meos_util.h` as globals. Files that only used those functions (e.g., `machinecontainer.cpp`, `infoserver.cpp`) can then drop the `gdioutput.h` include.

### 9. oEvent → Tab* Direct Coupling

`oEvent.cpp` has direct calls into UI Tab classes that must be decoupled:

| Call | Purpose | Replacement strategy |
|---|---|---|
| `TabList::baseButtons(gdi, 1, false)` | Renders UI buttons in list view | Callback `std::function<void(gdioutput&, int, bool)>` registered by TabList |
| `TabAuto::tabAutoKillMachines()` | Kills automatic timing machines | Callback `std::function<void()>` registered by TabAuto |
| `TabSI::getSI(gdiBase()).setSubSecondMode(use)` | Sets SportIdent sub-second mode | Callback `std::function<void(bool)>` registered by TabSI |

Expose callback typedefs (e.g., `BaseButtonsCallback`) on `oEvent.h` via `std::function`. UI classes register their callbacks during application initialization. This decouples the domain aggregate root from the UI layer.

### 10. Time Handling

- **TimeStamp.h/cpp**: Modernize to use standard `snprintf`/`swprintf` and standard C++ types.
- **timeconstants.hpp**: Centralize time-related constants (`timeUnitsPerSecond`, `timeConstSecPerHour`).
- **Win32 Time Shims**:
  - Shim `GetLocalTime`, `GetSystemTime` using `gettimeofday` and `localtime_r`/`gmtime_r`.
  - **CRITICAL**: `SystemTimeToFileTime` and `FileTimeToSystemTime` must be timezone-independent to match Win32 behavior. Use `timegm` instead of `mktime` to avoid local timezone shifts during round-trips.
- **Decoupling**: Utility classes like `TimeStamp` should not include heavy application headers like `meos.h`. Ensure they only depend on `StdAfx.h` and other necessary utilities.
- **Custom Epoch**: MeOS uses a custom 32-bit unsigned epoch relative to 2014-01-01 for `TimeStamp::Time`. This fits ~136 years of seconds in a 32-bit integer.

### 11. Redundant Overload Avoidance

On 64-bit Linux, `unsigned long` == `uint64_t`. Avoid redundant overloads for:
- `itos()` / `itow()` — don't provide both `unsigned long` and `uint64_t` versions

### 12. Dependency Management (vcpkg)

- **libmysql**: Building `libmysql` from source via vcpkg can be fragile on Linux environments due to heavy dependencies and build tool requirements (e.g., specific `ninja` versions). If it fails, consider using `libmariadb` or system-provided MySQL client libraries, or stubbing MySQL functionality for minimal builds.
- **Package Names**: Some vcpkg packages use `unofficial-` prefix for CMake targets (e.g., `unofficial-libharu`, `unofficial-minizip`).
- **vcpkg toolchain**: `CMAKE_TOOLCHAIN_FILE` must point to `vcpkg/scripts/buildsystems/vcpkg.cmake`.

## Test Infrastructure

- **GTest**: `find_package(GTest CONFIG REQUIRED)` + link `GTest::gtest GTest::gtest_main`.
- **Coverage**: Pass `-DCOVERAGE=ON` to CMake to enable `--coverage` flags for GCC/Clang.
- **Stubs**: Heavy coupling often requires stubs in `src/util/meos_stubs.cpp` to make modules like `util` or `domain` compile in isolation.
- **Data Container Initialization**: For domain object unit tests, ensure `oe->oControlData` (or relevant data container) is initialized before testing. The `oDataContainer` system requires explicit setup.
- **`StringCache` Initialization**: Ensure `StringCache` (used by `meos_util` string functions) is initialized via its constructor; otherwise, `wget()`/`get()` will segfault on empty vectors.
- **Protected Member Access**: For unit testing legacy classes, using `#define protected public` before including headers can be an effective way to access internal state without modifying the original code too much.
- **`enable_testing()`** must be in top-level `CMakeLists.txt`.

## Known Gotchas

0. **Legacy code is not static.** This is a fork that syncs with upstream. File contents, line numbers, function signatures, and even file names may change between migration runs. Always discover code structure dynamically rather than hardcoding assumptions.
1. **oEvent Method Scattering**: `oEvent` method implementations are often scattered across other domain entity files (e.g., `oEvent::fillControls` is in `oControl.cpp`). Always search the entire domain directory when looking for a method definition.
2. **Heavy Virtual Stubbing**: When stubbing heavily coupled classes like `oRunner` or `oTeam` to link a subset of the domain, you must implement ALL non-inline virtual methods declared in their headers to avoid missing vtable errors.
3. **CMake static libs need at least one source file** — use `*_dummy.cpp` placeholder.
4. **Localization** uses `#` as separator for substitutions. Placeholders `X, Y, Z, W` must be in both key and translation.
5. **Static members and globals** (like `lang`) need careful handling in modular builds.
6. **oEvent.cpp** is massive — don't try full migration in one step. Use a minimal skeleton initially.

## Useful Diagnostic Scripts

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
