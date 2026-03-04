add_executable(unit_tests 
    sample_test.cpp
    util_test.cpp
    localizer_test.cpp
    domain_test.cpp
    db_test.cpp
)

target_link_libraries(unit_tests PRIVATE 
    GTest::gtest 
    GTest::gtest_main
    db
    domain
    util
)
add_test(NAME unit_tests COMMAND unit_tests)
