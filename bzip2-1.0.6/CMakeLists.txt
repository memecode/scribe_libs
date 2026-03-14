cmake_minimum_required(VERSION 3.18)
include(../common.cmake)

add_library(bzip2 SHARED
    blocksort.c
    bzlib.c
    compress.c
    crctable.c
    decompress.c
    huffman.c
    randtable.c
    )

if (WIN32)
    set_target_properties(bzip2 PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

install(TARGETS bzip2
    RUNTIME
        DESTINATION ${CMAKE_BINARY_DIR}/lib
    LIBRARY
        DESTINATION ${CMAKE_BINARY_DIR}/lib)