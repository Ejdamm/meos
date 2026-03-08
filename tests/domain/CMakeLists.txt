add_executable(test_domain smoke_test.cpp)
target_link_libraries(test_domain PRIVATE domain GTest::gtest_main)
add_test(NAME test_domain COMMAND test_domain)
