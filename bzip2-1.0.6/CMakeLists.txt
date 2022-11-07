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