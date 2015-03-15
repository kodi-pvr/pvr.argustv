PROJECT(TSReader)

ENABLE_LANGUAGE(CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

# Source files
SET(SOURCES FileReader.cpp
            MultiFileReader.cpp
            TSReader.cpp)

# Header files
SET(HEADERS FileReader.h
            MultiFileReader.h
            TSReader.h)

SOURCE_GROUP("Header Files" FILES ${HEADERS})

ADD_LIBRARY(tsreader STATIC ${HEADERS} ${SOURCES})
