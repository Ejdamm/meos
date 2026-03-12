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

# Testing support for US-015
enable_testing()
