cmake_minimum_required(VERSION 3.20)
project(meos VERSION 2.4.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find dependencies
find_package(nlohmann_json CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)

# Executable
add_executable(meos src/main.cpp)

# Link dependencies
target_link_libraries(meos PRIVATE nlohmann_json::nlohmann_json SQLite::SQLite3)
