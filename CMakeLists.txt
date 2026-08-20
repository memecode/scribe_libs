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

set(DEPS "${CMAKE_CURRENT_SOURCE_DIR}")

function(GitProject NAME URI BRANCH ADDTARGET CLONE_ERR_FATAL)

    set(DIR "${DEPS}/${NAME}")
    message(STATUS "${NAME} = ${DIR}")
    if(NOT EXISTS ${DIR})
        execute_process(COMMAND git clone --recurse-submodules -b ${BRANCH} ${URI} ${DIR}
            RESULT_VARIABLE clone_result
            OUTPUT_VARIABLE clone_out)
        if(NOT ${clone_result} MATCHES "^[0-9]+$" OR ${clone_result})
            message(STATUS "git clone --recurse-submodules -b ${BRANCH} ${URI} ${DIR}")
            if(${CLONE_ERR_FATAL})
                message(FATAL_ERROR "${NAME} clone failed: ${clone_result} ${clone_out}")
            else()
                message(WARNING "${NAME} clone failed: ${clone_result} ${clone_out}")
                return()
            endif()
        endif()
    else()
        execute_process(COMMAND git pull
            WORKING_DIRECTORY ${DIR}
            RESULT_VARIABLE pull_result
            OUTPUT_VARIABLE pull_out)
        if(NOT ${pull_result} MATCHES "^[0-9]+$" OR ${pull_result})
            message(WARNING "${NAME} pull failed: ${pull_result} ${pull_out}")
        endif()
    endif()

    if(${ADDTARGET})
        set(LISTS "${DIR}/CMakeLists.txt")
        message("adding cmake: ${LISTS}")
        if(EXISTS ${LISTS})
            add_subdirectory(${DIR} ${NAME})
        else()
            message(STATUS "${NAME} is not a cmake project")
        endif()
    endif()

endfunction()

add_subdirectory(aspell-0.60.6.1)
add_subdirectory(Btree)
add_subdirectory(bzip2-1.0.6)

GitProject("libchardet" "https://github.com/Joungkyun/libchardet.git" "master" FALSE TRUE)
add_subdirectory(libchardet_cmake)
GitProject("litehtml" "https://github.com/memecode/litehtml.git" "master" TRUE TRUE)
