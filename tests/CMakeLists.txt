find_package(GTest REQUIRED)

macro(meos_add_test NAME)
    set(TEST_SRC ${ARGN})
    add_executable(${NAME} ${TEST_SRC})
    target_link_libraries(${NAME} PRIVATE GTest::gtest_main)
    add_test(NAME ${NAME} COMMAND ${NAME})
endmacro()

meos_add_test(smoke_test smoke_test.cpp)
