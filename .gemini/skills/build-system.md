# Build System Setup

## CMake + vcpkg Configuration

- Use `CMakePresets.json` version 6 for modern C++ projects.
- CMakePresets.json uses `$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake` for toolchain
- VCPKG_ROOT must be set before running cmake

### Setting VCPKG_ROOT

```bash
export VCPKG_ROOT=/home/adam.georgsson@fnox.it/vcpkg
```

### Commands

```bash
# Configure
cmake --preset debug
# or
cmake --preset release

# Build
cmake --build --preset debug
# or
cmake --build --preset release

# Test
ctest --test-dir build/debug --output-on-failure
```

## vcpkg Manifest Mode (vcpkg.json)

- Use \`builtin-baseline\` to specify the vcpkg version.
- **Gotcha:** If \`cmake --preset default\` fails with a baseline error (e.g., "failed to git show versions/baseline.json"), the local vcpkg repo might be outdated compared to the hash in \`vcpkg.json\`.
- **Solution:** Check the local vcpkg HEAD hash with \`cd $VCPKG_ROOT && git rev-parse HEAD\` and use that as the \`builtin-baseline\` in \`vcpkg.json\`.

## Project Structure

```
CMakeLists.txt          # Top-level: project(), add_subdirectory(), meos executable
CMakePresets.json       # default (Debug) and release (Release) presets
vcpkg.json              # Package manifest
src/
  main.cpp              # Entry point
  domain/
    CMakeLists.txt      # domain static library
  util/
    CMakeLists.txt      # util static library
```

Modules are **static libraries**. When adding new files, ensure they are added to the corresponding `CMakeLists.txt`. Dependencies between modules must be explicitly declared in `target_link_libraries`.

## Adding New Dependencies

1. Add to `vcpkg.json` dependencies array
2. Add `find_package()` in the relevant CMakeLists.txt
3. Link with `target_link_libraries()`

## Adding New Module Libraries

1. Create `src/<module>/CMakeLists.txt` with `add_library(<module> ...)`
2. Add `add_subdirectory(src/<module>)` to top-level CMakeLists.txt
3. Link to meos: `target_link_libraries(meos PRIVATE <module>)`
