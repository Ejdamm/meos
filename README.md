# MeOS - A Much Easier Orienteering System

MeOS is an orienteering system designed to be easy to use.

## Build Prerequisites

- **CMake** (3.24 or later)
- **Ninja**
- **C++ Compiler** (supporting C++20)
- **vcpkg** (with `VCPKG_ROOT` environment variable set)

## Build Instructions

This project uses CMake presets for configuration and building.

1.  **Configure the project:**
    ```bash
    cmake --preset debug
    # or
    cmake --preset release
    ```

2.  **Build the project:**
    ```bash
    cmake --build --preset debug
    # or
    cmake --build --preset release
    ```

## Run Instructions

After building, you can run the executable:

```bash
./build/debug/meos
# or
./build/release/meos
```

## Platform Notes

- **Linux**: Build tested on Ubuntu with GCC 13.
- **Windows**: Build using MSVC or GCC (MinGW).
- **macOS**: Build using Clang.
