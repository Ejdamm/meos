add_library(util STATIC util_dummy.cpp)
target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
