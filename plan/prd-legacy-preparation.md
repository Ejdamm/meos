# PRD: MeOS Legacy Code Preparation

## Introduction

This PRD covers the preparatory work needed in the legacy `code/` directory *before* migrating files to the new `src/` modular layout. The goal is to fix cross-platform blockers, reduce GUI coupling, and replace Win32-specific APIs in domain code.

This work can run **in parallel** with the main platform modernization effort (CMake setup, React frontend, database layer, etc.) since it operates exclusively on the legacy `code/` directory.

### Execution Environment

This PRD is executed by an autonomous agent running on **Linux Ubuntu**. The agent **cannot** run MSBuild or verify Windows builds locally. Changes are made "blind" — correctness of the Windows build is verified **manually via GitHub Actions CI** after the agent completes its work. The agent should:

- Focus on making correct, mechanical, safe transformations
- Use standard C++ replacements that are known to work on both MSVC and GCC/Clang
- Avoid MSVC-incompatible constructs (e.g., GCC-only extensions)
- Prefer conservative changes — when in doubt, use the safer option

### Context

MeOS's legacy codebase has several categories of issues that cause friction during migration to a cross-platform `src/` layout:

- **Case-sensitivity mismatches** in `#include` directives (Windows is case-insensitive, Linux is not)
- **Win32-specific string functions** (`_wtoi`, `sprintf_s`, `_itow_s`) used throughout domain code
- **Win32-specific types** (`DWORD`, `BOOL`) in domain code
- **Hardcoded backslash path separators** in file operations
- **Utility functions trapped in the GUI layer** (`widen()`, `narrow()`, `toUTF8()`, `fromUTF8()` live in `gdioutput.h`)
- **Direct coupling** from the domain aggregate root (`oEvent`) to UI classes (`TabList`, `TabAuto`, `TabSI`)

Fixing these in `code/` eliminates entire categories of migration errors and reduces the per-file effort when moving code to `src/`.

### Constraints

- All changes must be compatible with MSVC (Visual Studio 2022, v143) — verified post-hoc via CI, not locally
- No changes to the `src/` directory — this PRD operates exclusively on `code/`
- Changes should be mechanical/safe where possible (search-and-replace, move functions)
- Domain behavior must not change — these are refactoring-only changes
- Use only standard C++17 constructs that compile on both MSVC and GCC/Clang

## Goals

- Eliminate case-sensitivity mismatches in all `#include` directives
- Extract non-GUI utility functions from `gdioutput` so domain files can drop the GUI dependency
- Replace Win32-specific string functions with standard C++ equivalents in domain code
- Replace Win32-specific types with standard C++ types in domain code
- Normalize path separators to use `std::filesystem::path` or portable alternatives
- Decouple `oEvent` from Tab UI classes via callbacks or interfaces

## User Stories

### US-P0a: Fix Include Case Sensitivity

**Description:** Normalize all `#include` directives so the quoted filename matches the actual filename on disk. Windows is case-insensitive; Linux is not — these mismatches cause build failures on Linux.

**Acceptance Criteria:**
- [ ] Every `#include "..."` directive uses exactly the same casing as the file on disk
- [ ] A verification script confirms zero case mismatches
- [ ] Changes use only standard C++17 compatible with both MSVC and GCC/Clang (verified via CI post-hoc)

**Implementation Notes:**
- Can be largely automated with a script that compares `#include` strings against `ls` output
- Focus on `.h` and `.cpp` files in `code/`

### US-P0b: Extract Utility Functions from gdioutput

**Description:** Move the non-GUI utility functions `widen()`, `narrow()`, `toUTF8()`, `fromUTF8()` out of `gdioutput` to `meos_util.h/cpp` (or a new `string_util.h/cpp`). This breaks the artificial dependency from domain files to the GUI header.

**Acceptance Criteria:**
- [ ] `widen()`, `narrow()`, `toUTF8()`, `fromUTF8()` are available from `meos_util.h` (or a new header)
- [ ] Domain files that only needed these functions no longer include `gdioutput.h`
- [ ] `gdioutput` retains thin wrappers or `using` declarations for backward compatibility
- [ ] Changes use only standard C++17 compatible with both MSVC and GCC/Clang (verified via CI post-hoc)

**Implementation Notes:**
- Audit all domain files that include `gdioutput.h` — determine which actually use GUI functions vs. only string utilities
- Moving functions to `meos_util.h` is preferable to creating a new header (fewer changes needed)
- `gdioutput.h` should re-export or `using`-declare the moved functions so existing callers don't break

### US-P0c: Replace Win32-Specific String Functions in Domain Code

**Description:** Replace Windows-only string functions with standard C++ equivalents in domain files (`oEvent`, `oRunner`, `oClass`, `oDataContainer`, etc.).

**Acceptance Criteria:**
- [ ] No direct calls to `_wtoi`, `sprintf_s`, `swprintf_s`, `_itow_s` in domain `.cpp/.h` files
- [ ] Replacements use standard C++ (`<cstdlib>`, `<string>`, `<cwchar>`) or cross-platform wrappers
- [ ] Changes use only standard C++17 compatible with both MSVC and GCC/Clang (verified via CI post-hoc)

**Implementation Notes:**
- Common replacements:
  - `_wtoi(s)` -> `std::stoi(s)` or `wcstol(s, nullptr, 10)`
  - `sprintf_s(buf, fmt, ...)` -> `snprintf(buf, sizeof(buf), fmt, ...)`
  - `swprintf_s(buf, fmt, ...)` -> `swprintf(buf, sizeof(buf)/sizeof(wchar_t), fmt, ...)`
  - `_itow_s(val, buf, radix)` -> `std::to_wstring(val)` or `swprintf`
- Domain files include: `oEvent*.cpp`, `oRunner.cpp`, `oClass.cpp`, `oClub.cpp`, `oCourse.cpp`, `oControl.cpp`, `oCard.cpp`, `oTeam.cpp`, `oDataContainer.cpp`, `oBase.cpp`, `oFreePunch.cpp`
- UI files (Tab*, gdioutput) are **out of scope**

**Known Pitfalls:**
- When replacing `sprintf_s`/`swprintf_s` with `snprintf`/`swprintf`, always verify whether the buffer is a fixed-size array or a pointer — `sizeof(buf)` only works correctly on arrays, not pointers
- `lang.tl` returns `const wstring&`, which is directly compatible with `std::stoi` and `swprintf` — no intermediate conversion needed

### US-P0d: Replace Win32 Types with Standard Types in Domain Code

**Description:** Replace Win32-specific type aliases (`DWORD` -> `uint32_t`, `BOOL` -> `bool`) with standard C++ types in domain code. UI code (Tab*, gdioutput) is out of scope.

**Acceptance Criteria:**
- [ ] No `DWORD` or Win32 `BOOL` usage in domain `.cpp/.h` files
- [ ] `#include <cstdint>` added where needed
- [ ] Changes use only standard C++17 compatible with both MSVC and GCC/Clang (verified via CI post-hoc)

**Implementation Notes:**
- `DWORD` -> `uint32_t` (or `unsigned int` / `int` depending on usage context)
- Win32 `BOOL` -> `bool` (note: Win32 `BOOL` is `int`, so check for comparisons against `TRUE`/`FALSE`)
- May need to keep `#include <windows.h>` in some files temporarily if other Win32 APIs are still used

**Known Pitfalls:**
- `MeosSQL.cpp` contains `DWORD` and `BOOL` inside SQL string literals — these must NOT be replaced. Filter out replacements inside string constants

### US-P0e: Normalize Path Separators in Domain Code

**Description:** Replace hardcoded backslash (`\\`) path separators with cross-platform alternatives in domain code. Use `std::filesystem::path` where possible.

**Acceptance Criteria:**
- [ ] Domain files use `std::filesystem::path` or portable path construction
- [ ] Hardcoded `\\` in file paths replaced (non-escape-sequence uses)
- [ ] Changes use only standard C++17 compatible with both MSVC and GCC/Clang (verified via CI post-hoc)

**Implementation Notes:**
- `std::filesystem::path` handles platform-specific separators automatically
- `path / "subdir" / "file.ext"` is the idiomatic cross-platform way
- Be careful to distinguish path separators from escape sequences in strings
- `#include <filesystem>` and use `namespace fs = std::filesystem;`

**Known Pitfalls:**
- Backslashes in SQL quoting and library code (e.g., escape sequences) are NOT path separators — skip these
- Be careful with `parent_path()`: `parent_path()` of `"dir"` returns `""`, not a parent directory. To ensure a trailing separator, use `(p / "").wstring()`

### US-P0f: Decouple oEvent from Tab* UI Classes

**Description:** Remove `oEvent.cpp`'s direct coupling to Tab UI classes (`TabList`, `TabAuto`, `TabSI`). Currently `oEvent.cpp` has direct calls into Tab classes that create a hard dependency from the domain aggregate root to the UI layer. Replace with callbacks or interfaces so `oEvent` has no knowledge of Tab classes.

**Acceptance Criteria:**
- [ ] `oEvent.cpp` no longer includes `TabList.h`, `TabAuto.h`, or `TabSI.h`
- [ ] `TabList::baseButtons()` call replaced with a callback or interface
- [ ] `TabAuto::tabAutoKillMachines()` call replaced with a callback or interface
- [ ] `TabSI::getSI().setSubSecondMode()` call replaced with a callback or interface
- [ ] Tab classes register their callbacks during application initialization
- [ ] Changes use only standard C++17 compatible with both MSVC and GCC/Clang (verified via CI post-hoc)

**Implementation Notes:**
- Use `std::function` callbacks stored in `oEvent` — this pattern has already been proven in the migration work
- Tab classes register their callbacks during application startup (e.g., in `main()` or initialization code)
- This is the most complex preparation task — test thoroughly

**Known Pitfalls:**
- `meos.cpp` is the natural composition root where Tab callbacks should be registered (after `gEvent` initialization)
- After decoupling Tab includes, `oEvent` still has coupling to `meos.cpp` via external declarations of `createTabs` and `hideTabs` — these could also be converted to callbacks for full library extraction

## Functional Requirements

- FR-1: All changes must use standard C++17 compatible with MSVC (v143), GCC 12+, and Clang 15+ — Windows build verification happens post-hoc via CI
- FR-2: No behavioral changes to domain logic — all changes are structural/refactoring only
- FR-3: Domain files must not gain new Win32 dependencies as a result of these changes
- FR-4: Backward compatibility must be maintained for all public function signatures (use forwarding/wrappers where needed)

## Non-Goals

- Migrating any files to `src/` — that is covered by the main platform modernization PRD
- Changing UI/Tab code beyond what's needed for decoupling (US-P0f)
- Adding tests (the legacy codebase has no test infrastructure; tests come after migration)
- Fixing issues in non-domain files (UI, build scripts, etc.)

## Dependency Order

The stories can be worked largely in parallel, with one exception:

```
US-P0a (include casing)     — independent, do first
US-P0b (extract utilities)  — independent
US-P0c (string functions)   — independent, but easier after US-P0b
US-P0d (Win32 types)        — independent
US-P0e (path separators)    — independent
US-P0f (decouple oEvent)    — independent, but benefits from US-P0b being done
```

Recommended order: P0a first (quick, mechanical), then P0b (unblocks cleaner domain files), then P0c-P0e in any order, then P0f last (most complex).

## Success Metrics

- Zero case-sensitivity mismatches in `#include` directives
- Domain `.cpp/.h` files have no direct `#include "gdioutput.h"` (only via utility headers)
- `grep` for `_wtoi`, `sprintf_s`, `_itow_s`, `DWORD`, Win32 `BOOL` returns zero hits in domain files
- `oEvent.cpp` has zero includes of `Tab*.h` headers
- Windows MSBuild build succeeds after all changes (verified via CI post-hoc)
