cmake_minimum_required(VERSION 3.20)
project(meos VERSION 2.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(GTest CONFIG REQUIRED)

# Modules
add_subdirectory(src/util)
add_subdirectory(src/db)
add_subdirectory(src/domain)
add_subdirectory(src/net)
add_subdirectory(src/io)
add_subdirectory(src/app)

# Testing
enable_testing()
add_subdirectory(tests)

# Executable
add_executable(meos src/main.cpp)

# Link dependencies and modules
target_link_libraries(meos PRIVATE 
    util
    db
    domain
    net
    io
    app
    nlohmann_json::nlohmann_json 
    SQLite::SQLite3
)
