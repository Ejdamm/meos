add_executable(unit_tests 
    sample_test.cpp
    util_test.cpp
)

target_link_libraries(unit_tests PRIVATE 
    GTest::gtest 
    GTest::gtest_main
    util
)
add_test(NAME unit_tests COMMAND unit_tests)
