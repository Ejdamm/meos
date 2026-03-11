# MeOS Build Environment

## VCPKG Toolchain

The project uses vcpkg for dependency management. In the current environment, `VCPKG_ROOT` must be explicitly set for CMake to find the toolchain file.

### Setting VCPKG_ROOT

```bash
export VCPKG_ROOT=/home/adam.georgsson@fnox.it/vcpkg
```

### CMake Configuration

When configuring with CMake, use the default preset or provide the toolchain file path manually:

```bash
cmake --preset default
# OR
cmake -B build -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
```

### Build and Test

```bash
cmake --build --preset default
cd build && ctest
```

## Modular Structure

The project follows a modular structure in `src/`. When adding new files, ensure they are added to the corresponding `CMakeLists.txt` in the module directory. Dependencies between modules must be explicitly declared in `target_link_libraries`.
