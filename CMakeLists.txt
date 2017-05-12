project(pvr.argustv)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(Kodi REQUIRED)
find_package(kodiplatform REQUIRED)
find_package(p8-platform REQUIRED)

include_directories(${kodiplatform_INCLUDE_DIRS}
                    ${p8-platform_INCLUDE_DIRS}
                    ${KODI_INCLUDE_DIR}
                    ${PROJECT_SOURCE_DIR}/src)

if(NOT WIN32)
  add_options(ALL_LANGUAGES ALL_BUILDS "-fPIC")
endif()

# Source files
set(ARGUSTV_SOURCES src/activerecording.cpp
                    src/argustvrpc.cpp
                    src/channel.cpp
                    src/client.cpp
                    src/epg.cpp
                    src/EventsThread.cpp
                    src/guideprogram.cpp
                    src/KeepAliveThread.cpp
                    src/pvrclient-argustv.cpp
                    src/recording.cpp
                    src/recordinggroup.cpp
                    src/tools.cpp
                    src/upcomingrecording.cpp
                    src/uri.cpp
                    src/utils.cpp)

# Header files
set(ARGUSTV_HEADERS src/activerecording.h
                    src/argustvrpc.h
                    src/channel.h
                    src/client.h
                    src/epg.h
                    src/EventsThread.h
                    src/guideprogram.h
                    src/KeepAliveThread.h
                    src/pvrclient-argustv.h
                    src/recording.h
                    src/recordinggroup.h
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
endif(WIN32)

# Make sure that CMake adds the headers to the MSVC project
list(APPEND ARGUSTV_SOURCES ${ARGUSTV_HEADERS})

set(DEPLIBS ${p8-platform_LIBRARIES} tsreader)

find_package(JsonCpp REQUIRED)
list(APPEND DEPLIBS ${JSONCPP_LIBRARIES})
list(APPEND INCLUDES ${JSONCPP_INCLUDE_DIRS})

include_directories(${INCLUDES})

add_subdirectory(src/lib/tsreader)

build_addon(pvr.argustv ARGUSTV DEPLIBS)

include(CPack)
