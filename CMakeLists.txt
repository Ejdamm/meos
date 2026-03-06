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

add_executable(meos
    code/main_linux.cpp
    code/StdAfx.cpp
    code/gdioutput_stub.cpp
    code/meos_stubs.cpp
)

target_include_directories(meos PRIVATE code)

# Precompiled headers
target_precompile_headers(meos PUBLIC code/StdAfx.h)

# Link libraries
target_link_libraries(meos PRIVATE
    unofficial::libharu::hpdf
    unofficial::restbed::restbed
    unofficial::sqlite3::sqlite3
    nlohmann_json::nlohmann_json
    GTest::gtest
    PNG::PNG
    ZLIB::ZLIB
    unofficial::minizip::minizip
)
