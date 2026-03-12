# Testing Infrastructure Patterns

## Adding New Tests
Use the `meos_add_test` macro defined in the top-level `CMakeLists.txt`:
```cmake
meos_add_test(my_new_test source1.cpp source2.cpp)
```
This macro handles:
- Creating the executable target
- Linking against `GTest::gtest_main` (no main needed in source)
- Registering the test with CTest

## Coverage
To run tests with code coverage:
1. Configure with `-DMEOS_ENABLE_COVERAGE=ON`
2. Run tests with `ctest --test-dir build`
3. Use a tool like `gcovr` or `lcov` to generate reports.

## Conventions
- Test files should be named `*_test.cpp` and located in the `tests/` directory or appropriate subdirectories.
- Avoid manual `int main()` in tests; use Google Test's main library.
