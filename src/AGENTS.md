# Modern Source Layout (src/)

This directory contains the modernized, cross-platform source code for MeOS.

## Structure

- `app/`: Application-level services and main entry point.
- `db/`: Database abstraction layer (DAL) and implementations (e.g., SQLite).
- `domain/`: Core business logic and domain entities (`oRunner`, `oClass`, etc.).
- `io/`: Input/output operations, including file exports (CSV, XML, PDF).
- `net/`: Network services, including the REST API server.
- `util/`: Low-level utilities, string helpers, and parsers.

## CMake Integration

Each subdirectory contains its own `CMakeLists.txt` that defines a static library target of the same name.
Header include paths are configured so that files within a module can include each other using bare filenames.
The main executable (`meos`) links all these module libraries.

## Conventions

- Each module should be as self-contained as possible.
- Use `target_link_libraries` to define dependencies between modules (e.g., `domain` might depend on `util`).
- Avoid cross-module includes with relative paths (e.g., `../util/header.h`); instead, ensure the target include paths are correctly set in CMake.
