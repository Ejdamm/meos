add_library(util STATIC 
    meos_util.cpp 
    meos_util.h
    meosexception.h
    xmlparser.cpp
    xmlparser.h
    TimeStamp.cpp
    TimeStamp.h
    timeconstants.hpp
    localizer.cpp
    localizer.h
    inthashmap.h
    intkeymap.hpp
    intkeymapimpl.hpp
    random.cpp
    random.h
    win_types.h
)
target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(util PRIVATE stdc++fs)
