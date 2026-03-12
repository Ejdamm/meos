# MeOS Modernization

Modernized version of [MeOS](https://github.com/melinsoftware/meos).

## Build Prerequisites

- **CMake** (3.25 or newer)
- **Ninja**
- **vcpkg** (dependency manager)
- **C++20 compiler** (GCC 13+, Clang 16+, MSVC 19.34+)

## Build Instructions

1. Ensure the `VCPKG_ROOT` environment variable is set to your vcpkg installation directory.
2. Configure the project:
   ```bash
   cmake --preset default
   ```
3. Build the project:
   ```bash
   cmake --build build
   ```

## Run Instructions

After building, the executable can be found in the `build/` directory:
```bash
./build/meos
```

## Platform Notes

- **Linux:** Verified on Ubuntu with GCC 13 and Ninja.
- **Windows:** Supports MSVC with Ninja or Visual Studio generators via CMake presets.
