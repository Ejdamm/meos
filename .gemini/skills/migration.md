# Migration Patterns

## CMake & vcpkg

- Use **CMake Presets version 6** for standardized configuration.
- Always use **vcpkg manifest mode** (`vcpkg.json`).
- Reference `VCPKG_ROOT` via `$env{VCPKG_ROOT}` in `CMakePresets.json` to avoid hardcoding local paths.
- Enable `CMAKE_EXPORT_COMPILE_COMMANDS` for LSP support.
- Use `builtin-baseline` in `vcpkg.json` to ensure reproducible dependency versions.

## Codebase Discovery

- Do not assume fixed file paths or line numbers in legacy code.
- Use `grep_search` to find symbols and their declarations.
- Parse headers to understand class members and method signatures.
