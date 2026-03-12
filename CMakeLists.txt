cmake_minimum_required(VERSION 3.25)

project(meos
    VERSION 1.0.0
    DESCRIPTION "MeOS Modernization"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(meos src/main.cpp)

# --- Coverage Support (US-015) ---
option(MEOS_ENABLE_COVERAGE "Enable code coverage flags" OFF)
if(MEOS_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        add_compile_options(--coverage)
        add_link_options(--coverage)
    endif()
endif()

# --- Testing Infrastructure (US-015) ---
enable_testing()

macro(meos_add_test TEST_NAME)
    add_executable(${TEST_NAME} ${ARGN})
    target_link_libraries(${TEST_NAME} PRIVATE GTest::gtest_main)
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endmacro()

add_subdirectory(tests)
