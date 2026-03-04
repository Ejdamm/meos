find_package(SQLite3 REQUIRED)

add_library(db STATIC sqlitedatabase.cpp)
target_include_directories(db PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(db PUBLIC util SQLite::SQLite3)
