add_library(domain STATIC domain_dummy.cpp)
target_include_directories(domain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
