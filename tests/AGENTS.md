# Testing (tests/)

This directory contains unit and integration tests for MeOS.

## Structure

- `sample_test.cpp`: A basic sanity check test.
- `CMakeLists.txt`: Configures the `unit_tests` executable and adds it to `ctest`.

## Adding Tests

1. Create a new `.cpp` file (e.g., `my_module_test.cpp`).
2. Add `#include <gtest/gtest.h>` and your test cases.
3. Add the file to the `add_executable(unit_tests ...)` command in `tests/CMakeLists.txt`.
4. Link any required module libraries (e.g., `util`, `domain`) in `target_link_libraries`.

## Running Tests

Use the CMake preset:
```bash
ctest --preset default
```

Or run the binary directly for more control:
```bash
./build/tests/unit_tests --gtest_filter=MyTest.*
```
