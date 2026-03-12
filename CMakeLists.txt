cmake_minimum_required(VERSION 3.24)

project(meos
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# No module library targets — only the stub executable.
# No empty module directories (domain, util, db, etc.)
add_executable(meos src/main.cpp)
