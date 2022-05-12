project(scribeLibs)
cmake_minimum_required(VERSION 3.8)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)

add_subdirectory(libchardet)
add_subdirectory(../../../codelib/libjpeg-9a libjpeg-9a)
add_subdirectory(../../../codelib/libpng libpng)

#add_subdirectory(Btree)
#add_subdirectory(bzip2-1.0.6)