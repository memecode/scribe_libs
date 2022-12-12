cmake_minimum_required(VERSION 3.18)

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