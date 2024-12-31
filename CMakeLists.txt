cmake_minimum_required(VERSION 3.20)

set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
project(scribeLibs)

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

add_subdirectory(aspell-0.60.6.1)
add_subdirectory(libchardet)
add_subdirectory(Btree)
add_subdirectory(bzip2-1.0.6)
