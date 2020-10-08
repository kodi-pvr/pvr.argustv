cmake_minimum_required(VERSION 3.5)
project(TSReader)

# Source files
set(SOURCES FileReader.cpp
            MultiFileReader.cpp
            TSReader.cpp)

# Header files
set(HEADERS FileReader.h
            MultiFileReader.h
            TSReader.h)

source_group("Header Files" FILES ${HEADERS})

if(WIN32)
  add_compile_options(/wd4996)
endif()

add_library(tsreader STATIC ${HEADERS} ${SOURCES})
set_target_properties(tsreader PROPERTIES
    C_VISIBILITY_PRESET       hidden
    CXX_VISIBILITY_PRESET     hidden
    VISIBILITY_INLINES_HIDDEN YES
    POSITION_INDEPENDENT_CODE ON
)

