project(scribeLibs)
cmake_minimum_required(VERSION 3.8)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../codelib/libjpeg-9a")
    set(LIBJPEG ../../../codelib/libjpeg-9a)
elseif (EXISTS "${CMAKE_CURRENT_LIST_DIR}/../../../codelib/libjpeg")
    set(LIBJPEG ../../../codelib/libjpeg)
else()
    message(FATAL_ERROR "Can't find libjpeg")
endif()

add_subdirectory(libchardet)
add_subdirectory(libiconv-1.17)
add_subdirectory(${LIBJPEG} libjpeg-9a)
add_subdirectory(../../../codelib/libpng libpng)

#add_subdirectory(Btree)
#add_subdirectory(bzip2-1.0.6)