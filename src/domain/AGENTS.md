# Domain Module Learnings

## Data Containers and oBase
- All domain objects should inherit from `oBase`.
- Data storage is handled by `oDataContainer`. Each `oBase` object provides access to its data buffers via `getDataBuffers`.
- Use `oDataInterface` and `oDataConstInterface` for safe access to data fields.
- New data fields are added in `oDataContainer` using `addVariableInt`, `addVariableString`, etc.

## Dependencies and Stubs
- The domain module is highly coupled to `oEvent`, `gdioutput`, and `Table`.
- For the foundation to compile without the full UI and Server/DB modules, minimal stubs for these classes are provided in `domain_module.cpp`.
- When adding new domain classes, try to keep them decoupled from UI where possible, or use interfaces.

## Windows-Specific Functions
- Use `strcpy_s` and `wcsncpy_s` shims from `win_types.h` for cross-platform compatibility.
- `StringCache` is used for temporary `wstring` results to avoid many allocations, ensure it's initialized before use (now handled by constructor in `meos_util.h`).

## Codebase Patterns
- Every `#include` should match the disk casing.
- Use `domain_header.h` for common enums and forward declarations to reduce circular dependencies.
