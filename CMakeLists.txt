cmake_minimum_required(VERSION 3.20)
# vcpkg toolchain: pass -DCMAKE_TOOLCHAIN_FILE=$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake
# or use: cmake --preset=default  (see CMakePresets.json)
project(MeOS VERSION 5.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(MEOS_USE_MYSQL "Build with MySQL database support" ON)

if(MEOS_USE_MYSQL)
  find_package(unofficial-libmysql CONFIG REQUIRED)
endif()
find_package(haru CONFIG REQUIRED)
find_package(restbed CONFIG REQUIRED)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

# Source files grouped by module
set(SRC_APP
    src/app/meos.cpp
    src/app/meosversion.cpp
    src/app/MeOSFeatures.cpp
    src/app/StdAfx.cpp
    src/app/autotask.cpp
    src/app/recorder.cpp
)

set(SRC_DOMAIN
    src/domain/oBase.cpp
    src/domain/oCard.cpp
    src/domain/oClass.cpp
    src/domain/oClub.cpp
    src/domain/oControl.cpp
    src/domain/oCourse.cpp
    src/domain/oDataContainer.cpp
    src/domain/oEvent.cpp
    src/domain/oEventDraw.cpp
    src/domain/oEventResult.cpp
    src/domain/oEventSpeaker.cpp
    src/domain/oFreeImport.cpp
    src/domain/oFreePunch.cpp
    src/domain/oListInfo.cpp
    src/domain/oPunch.cpp
    src/domain/oReport.cpp
    src/domain/oRunner.cpp
    src/domain/oTeam.cpp
    src/domain/oTeamEvent.cpp
    src/domain/oevent_transfer.cpp
    src/domain/RunnerDB.cpp
    src/domain/classconfiginfo.cpp
    src/domain/generalresult.cpp
    src/domain/qualification_final.cpp
)

set(SRC_UI
    src/ui/gdioutput.cpp
    src/ui/TabAuto.cpp
    src/ui/TabBase.cpp
    src/ui/TabClass.cpp
    src/ui/TabClub.cpp
    src/ui/TabCompetition.cpp
    src/ui/TabControl.cpp
    src/ui/TabCourse.cpp
    src/ui/TabList.cpp
    src/ui/TabMulti.cpp
    src/ui/TabRunner.cpp
    src/ui/TabSI.cpp
    src/ui/TabSpeaker.cpp
    src/ui/TabTeam.cpp
    src/ui/Table.cpp
    src/ui/toolbar.cpp
    src/ui/progress.cpp
    src/ui/autocomplete.cpp
    src/ui/listeditor.cpp
    src/ui/methodeditor.cpp
    src/ui/metalist.cpp
    src/ui/prefseditor.cpp
    src/ui/qf_editor.cpp
    src/ui/animationdata.cpp
    src/ui/image.cpp
    src/ui/maprenderer.cpp
)

set(SRC_DB_NO_MYSQL
    src/db/no_mysql_stub.cpp
)

set(SRC_DB_MYSQL
    src/db/MeosSQL.cpp
    src/db/oEventSQL.cpp
    src/db/mysqlwrapper.cpp
    src/db/mysqldaemon.cpp
)

if(MEOS_USE_MYSQL)
  set(SRC_DB ${SRC_DB_MYSQL})
else()
  set(SRC_DB ${SRC_DB_NO_MYSQL})
endif()

set(SRC_NET
    src/net/restserver.cpp
    src/net/RestService.cpp
    src/net/onlineinput.cpp
    src/net/onlineresults.cpp
    src/net/liveresult.cpp
    src/net/infoserver.cpp
    src/net/socket.cpp
    src/net/download.cpp
)

set(SRC_IO
    src/io/iof30interface.cpp
    src/io/oImportExport.cpp
    src/io/importformats.cpp
    src/io/csvparser.cpp
    src/io/xmlparser.cpp
    src/io/HTMLWriter.cpp
    src/io/pdfwriter.cpp
    src/io/binencoder.cpp
)

set(SRC_HW
    src/hw/SportIdent.cpp
    src/hw/cardsystem.cpp
)

set(SRC_SPEAKER
    src/speaker/speakermonitor.cpp
)

set(SRC_PRINT
    src/print/printer.cpp
    src/print/printresultservice.cpp
)

set(SRC_UTIL
    src/util/meos_util.cpp
    src/util/TimeStamp.cpp
    src/util/parser.cpp
    src/util/localizer.cpp
    src/util/random.cpp
    src/util/utm.cpp
    src/util/machinecontainer.cpp
    src/util/newcompetition.cpp
    src/util/zip.cpp
    src/util/platform_socket.cpp
)

set(SRC_TEST
    test/testmeos.cpp
    test/tests.cpp
)

set(ALL_SOURCES
    ${SRC_APP}
    ${SRC_DOMAIN}
    ${SRC_UI}
    ${SRC_DB}
    ${SRC_NET}
    ${SRC_IO}
    ${SRC_HW}
    ${SRC_SPEAKER}
    ${SRC_PRINT}
    ${SRC_UTIL}
    ${SRC_TEST}
)

add_executable(MeOS WIN32 ${ALL_SOURCES} resources/meos.rc)

# Include directories — all src subdirs so #include "file.h" works without paths
target_include_directories(MeOS PRIVATE
    src
    src/app
    src/domain
    src/ui
    src/db
    src/net
    src/io
    src/hw
    src/speaker
    src/print
    src/util
    test
    resources
    third_party
)

# Compiler settings
target_compile_definitions(MeOS PRIVATE
    _WINDOWS
    UNICODE
    _UNICODE
    $<$<CONFIG:Debug>:WIN32;_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<BOOL:${MEOS_USE_MYSQL}>:MEOS_USE_MYSQL>
)

if(MSVC)
    target_compile_options(MeOS PRIVATE
        /W3                       # Warning level 3
        /wd4267 /wd4244 /wd4018  # Suppress size/sign conversion warnings
        /MP                       # Multi-processor compilation
        /std:c++17
    )
endif()

# Determine library directory based on platform and config
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(MEOS_LIB_DIR_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib/win64")
    set(MEOS_LIB_DIR_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib/win64_debug")
else()
    set(MEOS_LIB_DIR_RELEASE "${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib/win32")
    set(MEOS_LIB_DIR_DEBUG "${CMAKE_CURRENT_SOURCE_DIR}/third_party/lib/win32_debug")
endif()

target_link_directories(MeOS PRIVATE
    $<$<CONFIG:Debug>:${MEOS_LIB_DIR_DEBUG}>
    $<$<CONFIG:Release>:${MEOS_LIB_DIR_RELEASE}>
)

target_link_libraries(MeOS PRIVATE
    # Third-party libraries
    ZLIB::ZLIB
    haru::haru
    restbed::restbed
    PNG::PNG
)

if(MEOS_USE_MYSQL)
    target_link_libraries(MeOS PRIVATE unofficial::libmysql::libmysql)
endif()

if(WIN32)
    target_link_libraries(MeOS PRIVATE
        Msimg32
        comctl32
        odbc32
        odbccp32
        winmm
        ws2_32
        wininet
    )
else()
    target_link_libraries(MeOS PRIVATE pthread)
endif()

# DPI awareness manifest
set_target_properties(MeOS PROPERTIES
    VS_DPI_AWARE "PerMonitor"
)

# Source grouping for IDE (Visual Studio filters)
source_group("app"     FILES ${SRC_APP})
source_group("domain"  FILES ${SRC_DOMAIN})
source_group("ui"      FILES ${SRC_UI})
source_group("db"      FILES ${SRC_DB} ${SRC_DB_MYSQL} ${SRC_DB_NO_MYSQL})
source_group("net"     FILES ${SRC_NET})
source_group("io"      FILES ${SRC_IO})
source_group("hw"      FILES ${SRC_HW})
source_group("speaker" FILES ${SRC_SPEAKER})
source_group("print"   FILES ${SRC_PRINT})
source_group("util"    FILES ${SRC_UTIL})
source_group("test"    FILES ${SRC_TEST})

# -----------------------------------------------------------------------
# MeOS-server: headless REST server target (no Win32 UI dependencies)
# -----------------------------------------------------------------------

# Server util excludes newcompetition.cpp (<commctrl.h> + Tab* deps)
set(SRC_SERVER_UTIL
    src/util/meos_util.cpp
    src/util/TimeStamp.cpp
    src/util/parser.cpp
    src/util/localizer.cpp
    src/util/random.cpp
    src/util/utm.cpp
    src/util/machinecontainer.cpp
    src/util/zip.cpp
    src/util/platform_socket.cpp
)

# Server net subset: only core REST/info/socket (no UI-oriented net files)
set(SRC_SERVER_NET
    src/net/restserver.cpp
    src/net/infoserver.cpp
    src/net/socket.cpp
)

set(SRC_SERVER
    ${SRC_DOMAIN}
    ${SRC_IO}
    ${SRC_DB}
    ${SRC_SERVER_UTIL}
    ${SRC_SERVER_NET}
    src/ui/gdioutput.cpp
    src/ui/gdioutput_server_stubs.cpp
    src/ui/image_server_stub.cpp
    src/ui/progress.cpp
    src/app/recorder.cpp
    src/app/meosversion.cpp
    src/app/MeOSFeatures.cpp
    src/app/server_main.cpp
)

add_executable(MeOS-server ${SRC_SERVER})

target_include_directories(MeOS-server PRIVATE
    src
    src/app
    src/domain
    src/ui
    src/db
    src/net
    src/io
    src/hw
    src/speaker
    src/print
    src/util
    third_party
)

target_compile_definitions(MeOS-server PRIVATE
    MEOS_SERVER
    UNICODE
    _UNICODE
    $<$<CONFIG:Debug>:_DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<BOOL:${MEOS_USE_MYSQL}>:MEOS_USE_MYSQL>
)

if(MSVC)
    target_compile_options(MeOS-server PRIVATE
        /W3
        /wd4267 /wd4244 /wd4018
        /MP
        /std:c++17
    )
endif()

target_link_libraries(MeOS-server PRIVATE
    ZLIB::ZLIB
    haru::haru
    restbed::restbed
    PNG::PNG
)

if(MEOS_USE_MYSQL)
    target_link_libraries(MeOS-server PRIVATE unofficial::libmysql::libmysql)
endif()

if(WIN32)
    target_link_libraries(MeOS-server PRIVATE
        ws2_32
        odbc32
        odbccp32
    )
else()
    target_link_libraries(MeOS-server PRIVATE pthread)
endif()

source_group("domain"  FILES ${SRC_DOMAIN})
source_group("io"      FILES ${SRC_IO})
source_group("db"      FILES ${SRC_DB})
source_group("net"     FILES ${SRC_SERVER_NET})
source_group("util"    FILES ${SRC_SERVER_UTIL})
source_group("app"     FILES src/app/server_main.cpp src/app/recorder.cpp src/app/meosversion.cpp src/app/MeOSFeatures.cpp)
source_group("ui"      FILES src/ui/gdioutput.cpp src/ui/gdioutput_server_stubs.cpp src/ui/image_server_stub.cpp src/ui/progress.cpp)

# -----------------------------------------------------------------------
# MeOS-test: portable unit tests (no Win32/gdioutput dependencies)
# Covers IEventNotifier, NullNotifier, MockNotifier, platform_socket.
# Run on any platform with: cmake --build build --target MeOS-test && ./build/MeOS-test
# -----------------------------------------------------------------------
add_executable(MeOS-test
    test/portable_tests.cpp
    src/util/platform_socket.cpp
)

target_include_directories(MeOS-test PRIVATE
    src/domain
    src/app
    src/util
    test
)

if(MSVC)
    target_compile_options(MeOS-test PRIVATE /std:c++17 /W3)
    target_link_libraries(MeOS-test PRIVATE ws2_32)
else()
    target_compile_options(MeOS-test PRIVATE -std=c++17 -Wall)
endif()

enable_testing()
add_test(NAME portable-unit-tests COMMAND MeOS-test)
