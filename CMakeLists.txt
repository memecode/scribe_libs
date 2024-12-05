cmake_minimum_required(VERSION 3.20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
project(scribeLibs)

get_filename_component(CODELIB "${CMAKE_CURRENT_LIST_DIR}/../../../codelib" ABSOLUTE)

if (APPLE)
    execute_process(COMMAND xcodebuild -version
        OUTPUT_VARIABLE XcodeOut)
    separate_arguments(XcodeParts UNIX_COMMAND ${XcodeOut})
    list(GET XcodeParts 1 XCODE_VER)
    if (${XCODE_VER} VERSION_GREATER_EQUAL "13.0")
	    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
    else()
	    set(CMAKE_OSX_ARCHITECTURES "x86_64")
    endif()
    message("XCODE_VER=${XCODE_VER} CMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}")
	set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")
endif()

if(HAIKU)

    set(LIBJPEG_SO "/boot/system/develop/lib/libjpeg.so")
	if (NOT EXISTS ${LIBJPEG_SO})
		message(FATAL_ERROR "No libjpeg at: ${LIBJPEG_SO}")
	endif()
	add_library(libjpeg SHARED IMPORTED)

elseif(APPLE)

    include(ExternalProject)

    string(REPLACE ";" "$<SEMICOLON>" CMAKE_OSX_ARCHITECTURES_ "${CMAKE_OSX_ARCHITECTURES}")

    set(LIBJPEG_OUT "libjpeg.1.58.dylib")
    ExternalProject_Add(libjpeg
        PREFIX libjpeg
        GIT_REPOSITORY https://github.com/rbnj/libjpeg.git
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS=ON
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
            -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES_}
        INSTALL_COMMAND mkdir -p ${CMAKE_BINARY_DIR}/lib
            COMMAND cp ${LIBJPEG_OUT} ${CMAKE_BINARY_DIR}/lib/${LIBJPEG_OUT}
            COMMAND ln -sf ./${LIBJPEG_OUT} ${CMAKE_BINARY_DIR}/lib/libjpeg.1.dylib
            COMMAND ln -sf ./libjpeg.1.dylib ${CMAKE_BINARY_DIR}/lib/libjpeg.dylib)

    ExternalProject_Add(zlib
        PREFIX zlib
        GIT_REPOSITORY https://github.com/madler/zlib.git
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS=ON
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
            -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES_})

    ExternalProject_Add(libpng
        PREFIX libpng
        GIT_REPOSITORY https://github.com/memecode/libpng.git
        CMAKE_ARGS
            -DBUILD_SHARED_LIBS=ON
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
            -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES_}
            -DPNG_ARM_NEON=off
            -DPNG_STATIC=off
            -DPNG_SHARED=on
            -DPNG_BUILD_ZLIB=off
            -DZLIB_ROOT=${CMAKE_BINARY_DIR})

else()

    if (EXISTS "${CODELIB}/libjpeg-9a")
        set(LIBJPEG ${CODELIB}/libjpeg-9a)
    elseif (EXISTS "${CODELIB}/libjpeg")
        set(LIBJPEG ${CODELIB}/libjpeg)
    else()
        message(FATAL_ERROR "Can't find libjpeg")
    endif()

    if (EXISTS "${CODELIB}/libpng")
        set(LIBPNG ${CODELIB}/libpng)
    else()
        message(FATAL_ERROR "Can't find libpng")
    endif()

    if (EXISTS "${CODELIB}/lunasvg")
        set(LUNASVG ${CODELIB}/lunasvg)
    else()
        message(FATAL_ERROR "Can't find lunasvg")
    endif()

endif()

add_subdirectory(aspell-0.60.6.1)
add_subdirectory(libchardet)
add_subdirectory(libiconv-1.17)
add_subdirectory(Btree)
add_subdirectory(bzip2-1.0.6)
if(LIBJPEG)
    add_subdirectory(${LIBJPEG} libjpeg)
endif()
if(LIBPNG)
    add_subdirectory(${LIBPNG} libpng)
endif()
if(LUNASVG)
    add_subdirectory(${LUNASVG} lunasvg)
endif()