add_library(net STATIC net_dummy.cpp)
target_include_directories(net PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
