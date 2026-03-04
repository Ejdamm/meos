add_library(util STATIC 
    meos_util.cpp 
    meos_util.h
    meosexception.h
)
target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(util PRIVATE stdc++fs) # For std::filesystem if needed on older compilers, but GCC 13 should be fine.
# Actually, with GCC 13 and C++17, stdc++fs is often not needed, but doesn't hurt.
