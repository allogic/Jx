cmake_minimum_required(VERSION 3.8)

file(GLOB_RECURSE DECOMPILER_SOURCE ${DECOMPILER_DIR}/*.cpp)
file(GLOB LZOKAY_SOURCE ${LZOKAY_DIR}/lzokay.cpp)

add_executable(decompiler
  ${COMMON_SOURCE}
  ${DECOMPILER_SOURCE}
  ${LZOKAY_SOURCE}
)

set_property(TARGET decompiler PROPERTY CXX_STANDARD 23)

set_target_properties(decompiler
  PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${ROOT_DIR}/bin
  LIBRARY_OUTPUT_DIRECTORY ${ROOT_DIR}/bin
  RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}/bin
)

target_compile_definitions(decompiler
  PUBLIC _CRT_SECURE_NO_WARNINGS
)

target_include_directories(decompiler
  PUBLIC ${ROOT_DIR}
  PUBLIC ${DECOMPILER_DIR}
  PUBLIC ${LZOKAY_DIR}
)

target_link_directories(decompiler
  PUBLIC ${BIN_DIR}
)