cmake_minimum_required(VERSION 3.8)

project(jx VERSION 0.0.1 LANGUAGES C CXX)

set(ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set(COMMON_DIR ${ROOT_DIR}/common)
set(DATA_DIR ${DATA_DIR}/data)
set(BIN_DIR ${ROOT_DIR}/bin)
set(VENDOR_DIR ${ROOT_DIR}/vendor)

set(LZOKAY_DIR ${VENDOR_DIR}/lzokay)

set(DECOMPILER_DIR ${ROOT_DIR}/decompiler)

file(GLOB_RECURSE COMMON_SOURCE ${COMMON_DIR}/*.c ${COMMON_DIR}/*.cpp)

add_subdirectory("decompiler")