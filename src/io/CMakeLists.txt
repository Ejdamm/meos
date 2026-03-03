add_library(io STATIC io_dummy.cpp)
target_include_directories(io PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
