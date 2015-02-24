project(pvr.argustv)

cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR})

enable_language(CXX)

find_package(kodi REQUIRED)
find_package(kodiplatform REQUIRED)

include_directories(${kodiplatform_INCLUDE_DIRS}
                    ${KODI_INCLUDE_DIR}
                    ${PROJECT_SOURCE_DIR}/src)

if(NOT WIN32)
  add_options(ALL_LANGUAGES ALL_BUILDS "-fPIC")
endif()

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

set(DEPLIBS ${kodiplatform_LIBRARIES} tsreader)

find_package(JsonCpp REQUIRED)
list(APPEND DEPLIBS ${JSONCPP_LIBRARIES})
list(APPEND INCLUDES ${JSONCPP_INCLUDE_DIRS})

include_directories(${INCLUDES})

add_subdirectory(src/lib/tsreader)

build_addon(pvr.argustv ARGUSTV DEPLIBS)

include(CPack)
