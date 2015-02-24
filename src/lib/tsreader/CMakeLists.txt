PROJECT(TSReader)

ENABLE_LANGUAGE(CXX)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)

SET(SOURCES FileReader.cpp
            MultiFileReader.cpp
            TSReader.cpp)

ADD_LIBRARY(tsreader STATIC ${SOURCES})
