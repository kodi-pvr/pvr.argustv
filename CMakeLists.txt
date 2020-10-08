cmake_minimum_required(VERSION 3.5)
project(pvr.argustv)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

find_package(Kodi REQUIRED)

include_directories(${KODI_INCLUDE_DIR}/.. # Hack way with "/..", need bigger Kodi cmake rework to match right include ways
                    ${PROJECT_SOURCE_DIR}/src)

# Source files
set(ARGUSTV_SOURCES src/activerecording.cpp
                    src/addon.cpp
                    src/argustvrpc.cpp
                    src/channel.cpp
                    src/epg.cpp
                    src/EventsThread.cpp
                    src/guideprogram.cpp
                    src/KeepAliveThread.cpp
                    src/pvrclient-argustv.cpp
                    src/recording.cpp
                    src/recordinggroup.cpp
                    src/settings.cpp
                    src/tools.cpp
                    src/upcomingrecording.cpp
                    src/uri.cpp
                    src/utils.cpp)

# Header files
set(ARGUSTV_HEADERS src/activerecording.h
                    src/addon.h
                    src/argustvrpc.h
                    src/channel.h
                    src/epg.h
                    src/EventsThread.h
                    src/guideprogram.h
                    src/KeepAliveThread.h
                    src/pvrclient-argustv.h
                    src/recording.h
                    src/recordinggroup.h
                    src/settings.h
                    src/tools.h
                    src/upcomingrecording.h
                    src/uri.h
                    src/utils.h)
source_group("Header Files" FILES ${ARGUSTV_HEADERS})

if(WIN32)
# Misc files
set(RESOURCE_FILES pvr.argustv/addon.xml
                pvr.argustv/changelog.txt
                pvr.argustv/resources/settings.xml
                pvr.argustv/resources/language/resource.language.en_gb/strings.po)
source_group("Resource Files" FILES ${RESOURCE_FILES})
add_compile_options(/wd4996)
endif(WIN32)

# Make sure that CMake adds the headers to the MSVC project
list(APPEND ARGUSTV_SOURCES ${ARGUSTV_HEADERS})

set(DEPLIBS tsreader)

find_package(JsonCpp REQUIRED)
list(APPEND DEPLIBS ${JSONCPP_LIBRARIES})
list(APPEND INCLUDES ${JSONCPP_INCLUDE_DIRS})

include_directories(${INCLUDES})

add_subdirectory(src/lib/tsreader)

build_addon(pvr.argustv ARGUSTV DEPLIBS)

# Temp workaround, this becomes later added to kodi-dev-kit system
set_target_properties(pvr.argustv PROPERTIES
    C_VISIBILITY_PRESET       hidden
    CXX_VISIBILITY_PRESET     hidden
    VISIBILITY_INLINES_HIDDEN YES
)

include(CPack)
