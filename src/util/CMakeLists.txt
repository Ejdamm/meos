add_library(util STATIC 
    meos_util.cpp 
    meos_util.h
    meosexception.h
    xmlparser.cpp
    xmlparser.h
    csvparser.cpp
    csvparser.h
    TimeStamp.cpp
    TimeStamp.h
    timeconstants.hpp
    SICard.h
    localizer.cpp
    localizer.h
)
target_include_directories(util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(util PRIVATE stdc++fs)
