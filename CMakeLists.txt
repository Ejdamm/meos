cmake_minimum_required(VERSION 3.16)
project(MeOS CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies via vcpkg
find_package(unofficial-libharu CONFIG REQUIRED)
find_package(unofficial-restbed CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)
find_package(unofficial-minizip CONFIG REQUIRED)

# Common include path for all targets to support legacy bare includes
set(MEOS_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    ${CMAKE_CURRENT_SOURCE_DIR}/src/domain
    ${CMAKE_CURRENT_SOURCE_DIR}/src/net
    ${CMAKE_CURRENT_SOURCE_DIR}/src/db
    ${CMAKE_CURRENT_SOURCE_DIR}/src/util
    ${CMAKE_CURRENT_SOURCE_DIR}/src/io
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

# Utility module
add_library(util STATIC
    src/util/StdAfx.cpp
    src/util/gdioutput_stub.cpp
    src/util/meos_stubs.cpp
    src/util/random.cpp
    src/util/TimeStamp.cpp
    src/util/binencoder.cpp
    src/util/utm.cpp
    src/util/meos_util.cpp
    src/util/xmlparser.cpp
    src/util/csvparser.cpp
    src/util/localizer.cpp
    src/util/owordlist.cpp
)
target_include_directories(util PUBLIC ${MEOS_INCLUDE_DIRS})
target_precompile_headers(util PUBLIC src/util/StdAfx.h)

# Domain module
add_library(domain STATIC
    src/domain/domain_module.cpp
    src/domain/oBase.cpp
    src/domain/oDataContainer.cpp
    src/domain/oControl.cpp
    src/domain/oPunch.cpp
)
target_include_directories(domain PUBLIC ${MEOS_INCLUDE_DIRS})
target_link_libraries(domain PUBLIC util)

# Database module
add_library(db STATIC
    src/db/db_module.cpp
)
target_include_directories(db PUBLIC ${MEOS_INCLUDE_DIRS})
target_link_libraries(db PUBLIC domain)

# Network module
add_library(net STATIC
    src/net/net_module.cpp
)
target_include_directories(net PUBLIC ${MEOS_INCLUDE_DIRS})
target_link_libraries(net PUBLIC domain)

# IO module
add_library(io STATIC
    src/io/io_module.cpp
)
target_include_directories(io PUBLIC ${MEOS_INCLUDE_DIRS})
target_link_libraries(io PUBLIC domain)

# UI module (legacy Win32/GDI)
add_library(ui STATIC
    src/ui/ui_module.cpp
)
target_include_directories(ui PUBLIC ${MEOS_INCLUDE_DIRS})
target_link_libraries(ui PUBLIC domain util)

# Main executable
add_executable(meos
    src/app/main_linux.cpp
)

target_link_libraries(meos PRIVATE
    util
    domain
    db
    net
    io
    ui
    unofficial::libharu::hpdf
    unofficial::restbed::restbed
    unofficial::sqlite3::sqlite3
    nlohmann_json::nlohmann_json
    GTest::gtest
    PNG::PNG
    ZLIB::ZLIB
    unofficial::minizip::minizip
)

# --- Test Infrastructure ---
enable_testing()

option(COVERAGE "Enable code coverage reporting" OFF)

if(COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    endif()
endif()

add_executable(smoke_test tests/smoke_test.cpp)
target_link_libraries(smoke_test PRIVATE GTest::gtest GTest::gtest_main)
add_test(NAME SmokeTest COMMAND smoke_test)

add_executable(util_test tests/util_test.cpp)
target_link_libraries(util_test PRIVATE util GTest::gtest GTest::gtest_main)
add_test(NAME UtilTest COMMAND util_test)

add_executable(domain_test tests/domain_test.cpp)
target_link_libraries(domain_test PRIVATE domain GTest::gtest GTest::gtest_main)
add_test(NAME DomainTest COMMAND domain_test)

add_executable(db_test tests/db_test.cpp)
target_link_libraries(db_test PRIVATE db GTest::gtest GTest::gtest_main)
add_test(NAME DBTest COMMAND db_test)

add_executable(net_test tests/net_test.cpp)
target_link_libraries(net_test PRIVATE net GTest::gtest GTest::gtest_main)
add_test(NAME NetTest COMMAND net_test)

add_executable(io_test tests/io_test.cpp)
target_link_libraries(io_test PRIVATE io GTest::gtest GTest::gtest_main)
add_test(NAME IOTest COMMAND io_test)

# Note: As modules are fully migrated, they will be populated with all files.
# Currently, most modules are stubs to maintain cross-platform build success.
