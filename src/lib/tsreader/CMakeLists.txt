cmake_minimum_required(VERSION 3.5)
PROJECT(TSReader)

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
