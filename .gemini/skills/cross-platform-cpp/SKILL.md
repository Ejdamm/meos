# Skill: Cross-Platform C++ Porting (Win32 to Standard C++)

Use this skill when porting Win32/MSVC code to compile on Linux (GCC/Clang).

## Include casing
- Linux filesystems are case-sensitive. Every `#include "..."` must match the exact filename on disk.
- `stdafx.h` vs `StdAfx.h` is the most common mismatch.
- Run `python3 verify_includes.py` to check for mismatches.

## GCC is stricter than MSVC

### Incomplete types in containers
GCC requires complete type definitions for classes used in `std::vector`, even if MSVC accepts forward declarations. Provide at least a minimal class definition (even an empty class body) in a reachable header.

### Virtual method coverage
All virtual methods of a class must have implementations (even empty stubs) for GCC to generate the vtable. Missing implementations cause linker errors like "undefined reference to vtable for ClassName".

### Template implementations
Template implementation files (`*impl.hpp`) must be explicitly `#include`-d at the end of their header files. MSVC sometimes resolves these through other paths; GCC does not.

## String functions

| Win32 | Standard C++ replacement |
|-------|-------------------------|
| `_wtoi(s)` | `wcstol(s, nullptr, 10)` |
| `_wtof(s)` | `wcstod(s, nullptr)` |
| `_wtoi64(s)` | `wcstoll(s, nullptr, 10)` |
| `_itow_s(v, buf, sz, 10)` | `swprintf(buf, sz, L"%d", v)` |
| `sprintf_s(buf, fmt, ...)` | `snprintf(buf, sizeof(buf), fmt, ...)` |
| `swprintf_s(buf, fmt, ...)` | `swprintf(buf, sizeof(buf)/sizeof(wchar_t), fmt, ...)` |
| `_stricmp` / `_wcsicmp` / `lstrcmpi` | `compareStringIgnoreCase()` from `meos_util.h` |
| `strcpy_s` / `wcsncpy_s` | Shims in `win_types.h` |

Always specify buffer size explicitly with `snprintf`/`swprintf` — MSVC's `_s` variants infer size differently.

## Types

| Win32 | Standard C++ | Header needed |
|-------|-------------|---------------|
| `DWORD` | `uint32_t` | `<cstdint>` |
| `WORD` | `uint16_t` | `<cstdint>` |
| `BYTE` | `uint8_t` | `<cstdint>` |
| `BOOL` | `bool` | (builtin) |
| `INT` | `int` | (builtin) |
| `LPWSTR` | `wchar_t*` | (builtin) |

Exception: Leave `BOOL` in SQL query strings — it's a MySQL/SQLite column type.

## Filesystem

| Win32 | Standard C++ |
|-------|-------------|
| `_wsplitpath_s` | `std::filesystem::path::stem()`, `parent_path()`, `extension()` |
| `GetFileAttributes` | `std::filesystem::exists()` |
| Hardcoded `\\` | `std::filesystem::path` with `/` |

Note: `parent_path()` does not include a trailing separator — legacy concatenation code may expect one.

## Time functions
- `SystemTimeToFileTime` / `FileTimeToSystemTime` shims exist in `win_types.h`.
- Use `timegm` (not `mktime`) in shims for consistent UTC-like behavior. `mktime` applies local timezone offset.
- `GetLocalTime` shim uses `localtime_r` on Linux.

## Locale
- Set locale to `"C.UTF-8"` early (in `main()` or test setup).
- Without this, `wcscasecmp` and `towlower` fail for non-ASCII characters (Swedish a/o/a).
- `std::setlocale(LC_ALL, "C.UTF-8")` or `setlocale(LC_ALL, "C.UTF-8")`.

## String conversion (widen/narrow/UTF-8)
- Use global functions from `meos_util.h`: `widen()`, `narrow()`, `toUTF8()`, `fromUTF8()`, `recodeToWide()`, `recodeToNarrow()`.
- Implemented via `std::wstring_convert` (deprecated but functional — no external deps needed).
- Never include `gdioutput.h` just for string conversion.

## Multi-character literals
- MSVC accepts `'--'` and `'x'` as multi-char constants.
- Replace with wide character literals: `L'\u2013'` (en-dash), `L'\u00D7'` (multiplication sign).

## UI decoupling
- Domain code must not include UI headers (`Tab*.h`, `gdioutput.h`).
- Use `std::function` callbacks registered during app initialization to break domain-to-UI dependencies.
- Pattern: domain class stores `std::function<void()>` member, UI layer sets it via setter, domain calls it when needed.
