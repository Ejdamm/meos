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

## Testing and Coverage

### C++ Tests (Google Test)

- Use vcpkg manifest mode to add `gtest` dependency.
- Define tests in the `tests/` directory with a local `CMakeLists.txt`.
- Use a macro for consistent test registration:
```cmake
macro(meos_add_test NAME)
    set(TEST_SRC ${ARGN})
    add_executable(${NAME} ${TEST_SRC})
    target_link_libraries(${NAME} PRIVATE GTest::gtest_main)
    add_test(NAME ${NAME} COMMAND ${NAME})
endmacro()
```
- Link against `GTest::gtest_main` to automatically include a `main()` function for tests.

### Running Tests

```bash
# From build directory
ctest
```

### Code Coverage (GCC/Clang)

- Enable via CMake option: `-DMEOS_ENABLE_COVERAGE=ON`.
- Apply flags to both compile and link steps:
```cmake
if(MEOS_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage)
        add_link_options(--coverage)
    endif()
endif()
```
- Running the tests will generate `.gcno` (at compile time) and `.gcda` (at runtime) files.

Modules are **static libraries**. When adding new files, ensure they are added to the corresponding `CMakeLists.txt`. Dependencies between modules must be explicitly declared in `target_link_libraries`.

## Adding New Dependencies

1. Add to `vcpkg.json` dependencies array
2. Add `find_package()` in the relevant CMakeLists.txt
3. Link with `target_link_libraries()`

## Adding New Module Libraries

1. Create `src/<module>/CMakeLists.txt` with `add_library(<module> ...)`
2. Add `add_subdirectory(src/<module>)` to top-level CMakeLists.txt
3. Link to meos: `target_link_libraries(meos PRIVATE <module>)`
