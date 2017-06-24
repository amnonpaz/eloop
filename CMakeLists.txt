cmake_minimum_required(VERSION 3.5)
project(eLoop)

option(BUILD_TESTS "Build test programs" OFF)

set(ELOOP_LIB eloop)
set(ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(BUILD_DIR ${ROOT_DIR}/build/)

message("Root dir: " ${ROOT_DIR})
 
file(GLOB SOURCES src/*.c)
include_directories(${ROOT_DIR}/include)

if (${BUILD_TESTS})
    add_subdirectory(tests)
endif(${BUILD_TESTS})

add_library(${ELOOP_LIB} SHARED ${SOURCES})
set_target_properties(${ELOOP_LIB}
    PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${BUILD_DIR}
)
