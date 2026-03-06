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
)
target_include_directories(util PUBLIC ${MEOS_INCLUDE_DIRS})
target_precompile_headers(util PUBLIC src/util/StdAfx.h)

# Domain module
add_library(domain STATIC
    src/domain/domain_module.cpp
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

# Note: As modules are fully migrated, they will be populated with all files.
# Currently, most modules are stubs to maintain cross-platform build success.
