# String Migration Skill

## Win32 to Standard C++ String Functions

When migrating from Win32-specific string functions to standard C++, follow these rules:

### Integers to Strings
- `_wtoi(some_wstring.c_str())` -> `std::stoi(some_wstring)`
- `_wtoi(some_ptr)` -> `std::stoi(some_ptr)` (ensure it's `wchar_t*`)
- `_wtoi64(...)` -> `std::stoll(...)`
- `_itow_s(val, bf, size, 10)` -> `swprintf(bf, size, L"%d", val)`

Note: `std::stoi` can throw if the string is not a valid integer. If behavioral parity with `_wtoi` (which returns 0 on failure) is required, use `wcstol` with a null pointer for the endptr and 10 for the radix. However, in most MeOS logic, `std::stoi` is acceptable.

### Formatted Output
- `sprintf_s(bf, format, ...)` -> `snprintf(bf, sizeof(bf), format, ...)`
- `sprintf_s(bf, size, format, ...)` -> `snprintf(bf, size, format, ...)`
- `swprintf_s(bf, L"format", ...)` -> `swprintf(bf, sizeof(bf)/sizeof(wchar_t), L"format", ...)`
- `swprintf_s(bf, size, L"format", ...)` -> `swprintf(bf, size, L"format", ...)`

### Standard Headers
Always ensure the following headers are included:
- `<string>` for `std::stoi`, `std::stoll`
- `<cstdio>` for `snprintf`
- `<cwchar>` for `swprintf`

### Codebase Patterns
- MeOS often uses `lang.tl("key")` which returns a `const wstring &`.
- Buffers are commonly named `bf`, `wbf`, `bf2`, `title`.
- Many formatted string operations are used for generating XML, HTML, or UI labels.
