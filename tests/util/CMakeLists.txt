add_executable(test_util smoke_test.cpp)
target_link_libraries(test_util PRIVATE util GTest::gtest_main)
add_test(NAME test_util COMMAND test_util)
