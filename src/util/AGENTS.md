# Util Module

The `util` module provides general-purpose utilities, including time/date operations, string conversions, and cross-platform helpers.

## Core Utilities

### meos_util.h / meos_util.cpp

Provides various utility functions:
- **Time/Date**: `getLocalTime`, `formatTime`, `convertDateYMD`, etc.
- **Parsing**: `xmlparser` and `csvparser` are available for high-level data processing.
- **Time**: `TimeStamp` and `timeconstants.hpp` provide platform-independent time handling.
- **Initialization**: `StringCache` must be initialized (handled by constructor now).
- **Migration**: `csvparser` currently has stubs for domain-dependent methods (`oEvent` etc) until those modules are migrated.
- **Conversions**: `string2Wide`, `wide2String`.
- **Files**: `fileExists`, `moveFile`.

### inthashmap.h / intkeymap.hpp

Provides a custom high-performance hash map implementation:
- `intkeymap<T, KEY>`: Template for a hash map with custom key and value types.
- `inthashmap`: A typedef for `intkeymap<int, int>`.
- The implementation is in `intkeymapimpl.hpp` and is typically included in `.cpp` files where it is used.

### meosexception.h

Defines `meosException` and `meosCancel` for error handling and cancellation.

### localizer.h / localizer.cpp

The `Localizer` class provides the localization system for MeOS. It is available as a global `extern Localizer lang;`.
- **Initialization**: Call `lang.init()` at the beginning and `lang.unload()` at the end.
- **Translation**: Use `lang.tl(L"String")` to translate a wide string or `lang.tl("String")` for a normal string.
- **Substitutions**: Use `#` to separate key and values for substitution: `lang.tl(L"Sträcka X#" + itow(leg))`. The translation table must contain the placeholder (e.g., `X`, `Y`, `Z`, `W`).
- **Resource Loading**: Add a language resource using `lang.get().addLangResource(L"Name", L"path/to/file.lng")` and load it using `lang.get().loadLangResource(L"Name")`.
- **Files**: `.lng` files are expected to be in UTF-8 format.

## Cross-Platform Implementation

The module has been refactored to be platform-independent:
- **SYSTEMTIME**: A custom struct is defined on non-Windows platforms to maintain compatibility with legacy code.
- **Time**: Uses `std::chrono` and `std::tm` (via `localtime_r`) for local time operations.
- **Files**: Uses `std::filesystem` for file existence and move operations.
- **Strings**: Uses standard `snprintf`, `swprintf`, and `std::to_string`/`std::to_wstring`.

## Usage Notes

- Always use `std::` prefix for standard library types.
- Wide strings (`std::wstring`) are preferred for user-facing content.
- `string2Wide` and `wide2String` are currently implemented as simple conversions.
- Win32-specific functions like `expandDirectory` are currently stubbed or simplified.
- Avoid adding new Win32-specific dependencies to this module.
