project(sync)
cmake_minimum_required(VERSION 2.6)
add_definitions("-std=gnu99")

set(SOURCES data.c device.c track.c)
set(HEADERS data.h device.h track.h sync.h base.h) 
add_library(sync SHARED ${SOURCES} ${HEADERS})
