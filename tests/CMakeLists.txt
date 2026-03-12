# US-015: Test Infrastructure
find_package(GTest REQUIRED)

meos_add_test(smoke_test smoke_test.cpp)
