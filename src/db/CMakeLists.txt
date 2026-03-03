add_library(db STATIC db_dummy.cpp)
target_include_directories(db PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
