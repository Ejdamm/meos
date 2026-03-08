# Testing Infrastructure

This directory contains C++ unit tests for the modernized MeOS project.

## Conventions

- **Framework:** [Google Test (gtest)](https://github.com/google/googletest)
- **Structure:** Mirror the `src/` directory structure.
  - `src/domain/` -> `tests/domain/`
  - `src/util/` -> `tests/util/`
- **Naming:**
  - Test executables: `test_<module>` (e.g., `test_domain`)
  - Test files: `smoke_test.cpp` for initial verification, then `<class>_test.cpp` for specific classes.
- **Main Entry Point:** Use `GTest::gtest_main` to avoid writing `main()` in every test file.

## Adding a New Test

1. Create a `.cpp` file in the appropriate subdirectory of `tests/`.
2. Update the `CMakeLists.txt` in that subdirectory:
   ```cmake
   add_executable(test_<name> <file>.cpp)
   target_link_libraries(test_<name> PRIVATE <module_library> GTest::gtest_main)
   add_test(NAME test_<name> COMMAND test_<name>)
   ```
3. Run tests using `ctest --test-dir build`.

## Smoke Tests

Every module should have at least one `smoke_test.cpp` that verifies basic compilation and linking:

```cpp
#include <gtest/gtest.h>

TEST(ModuleNameSmoke, Compiles) {
    EXPECT_TRUE(true);
}
```
