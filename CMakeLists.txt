cmake_minimum_required(VERSION 3.25)
project(meos LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_TESTING "Build tests" ON)

if(BUILD_TESTING)
  enable_testing()
endif()

add_subdirectory(src/domain)
add_subdirectory(src/util)

add_executable(meos src/main.cpp)
target_link_libraries(meos PRIVATE domain util)
target_precompile_headers(meos PRIVATE
  <string>
  <vector>
  <map>
  <memory>
)
