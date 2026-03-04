add_library(domain STATIC 
    oBase.cpp 
    oBase.h
    oDataContainer.cpp
    oDataContainer.h
    oControl.cpp
    oControl.h
    oPunch.cpp
    oPunch.h
    TableType.h
)
target_include_directories(domain PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(domain PUBLIC util)
