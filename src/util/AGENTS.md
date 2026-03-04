# Util Module

The `util` module provides general-purpose utilities, including time/date operations, string conversions, and cross-platform helpers.

## Core Utilities

### meos_util.h / meos_util.cpp

Provides various utility functions:
- **Time/Date**: `getLocalTime`, `formatTime`, `convertDateYMD`, etc.
- **String**: `itos`, `itow`, `split`, `trim`, `encodeXML`, etc.
- **Conversions**: `string2Wide`, `wide2String`.
- **Files**: `fileExists`, `moveFile`.

### meosexception.h

Defines `meosException` and `meosCancel` for error handling and cancellation.

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
