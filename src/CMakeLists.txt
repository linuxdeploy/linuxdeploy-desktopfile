cmake_minimum_required(VERSION 3.0)

file(GLOB HEADERS ${PROJECT_SOURCE_DIR}/include/linuxdeploy/desktopfile/*.h)

add_library(linuxdeploy_desktopfile STATIC
    desktopfile.cpp
    desktopfileentry.cpp
    desktopfilereader.cpp
    desktopfilereader.h
    desktopfilewriter.cpp
    desktopfilewriter.h
    util.h
    ${HEADERS}
)

target_include_directories(linuxdeploy_desktopfile PUBLIC ${PROJECT_SOURCE_DIR}/include)
