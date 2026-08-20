project(libchardet)
cmake_minimum_required(VERSION 3.20)

include(../common.cmake)

get_filename_component( CHARDET_DIR
                        "${CMAKE_CURRENT_SOURCE_DIR}/../libchardet"
                        ABSOLUTE)
if (NOT EXISTS ${CHARDET_DIR})
    message(FATAL_ERROR "libchardet source directory not found: ${CHARDET_DIR}")
endif()

add_library(chardet SHARED
    ${CHARDET_DIR}/src/chardet.cpp
    ${CHARDET_DIR}/src/CharDistribution.cpp
    ${CHARDET_DIR}/src/JpCntx.cpp
    ${CHARDET_DIR}/src/nsBig5Prober.cpp
    ${CHARDET_DIR}/src/nsCharSetProber.cpp
    ${CHARDET_DIR}/src/nsEscCharsetProber.cpp
    ${CHARDET_DIR}/src/nsEscSM.cpp
    ${CHARDET_DIR}/src/nsEUCJPProber.cpp
    ${CHARDET_DIR}/src/nsEUCKRProber.cpp
    ${CHARDET_DIR}/src/nsEUCTWProber.cpp
    ${CHARDET_DIR}/src/nsGB2312Prober.cpp
    ${CHARDET_DIR}/src/nsHebrewProber.cpp
    ${CHARDET_DIR}/src/nsLatin1Prober.cpp
    ${CHARDET_DIR}/src/nsMBCSGroupProber.cpp
    ${CHARDET_DIR}/src/nsMBCSSM.cpp
    ${CHARDET_DIR}/src/nsSBCharSetProber.cpp
    ${CHARDET_DIR}/src/nsSBCSGroupProber.cpp
    ${CHARDET_DIR}/src/nsSJISProber.cpp
    ${CHARDET_DIR}/src/nsUniversalDetector.cpp
    ${CHARDET_DIR}/src/nsUTF8Prober.cpp
    ${CHARDET_DIR}/src/tables/LangCyrillicModel.cpp
    ${CHARDET_DIR}/src/tables/LangGreekModel.cpp
    ${CHARDET_DIR}/src/tables/LangBulgarianModel.cpp
    ${CHARDET_DIR}/src/tables/LangHungarianModel.cpp
    ${CHARDET_DIR}/src/tables/LangThaiModel.cpp
    ${CHARDET_DIR}/src/tables/LangHebrewModel.cpp
    ${CHARDET_DIR}/src/tables/LangArabicModel.cpp
    ${CHARDET_DIR}/src/tables/LangDanishModel.cpp
    ${CHARDET_DIR}/src/tables/LangEsperantoModel.cpp
    ${CHARDET_DIR}/src/tables/LangFrenchModel.cpp
    ${CHARDET_DIR}/src/tables/LangGermanModel.cpp
    ${CHARDET_DIR}/src/tables/LangSpanishModel.cpp
    ${CHARDET_DIR}/src/tables/LangTurkishModel.cpp
    ${CHARDET_DIR}/src/tables/LangVietnameseModel.cpp
    )

target_include_directories(chardet PUBLIC ${CHARDET_DIR}/include ${CHARDET_DIR}/src)
target_compile_definitions(chardet PRIVATE -DHAVE_DLL_EXPORT)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")

if (WIN32)

    set_target_properties(chardet PROPERTIES
                            OUTPUT_NAME_DEBUG "chardet${VSYEAR}x${BITSIZE}d"
                            OUTPUT_NAME_RELEASE "chardet${VSYEAR}x${BITSIZE}")

    get_filename_component(LIB_DIR "../../../Lgi/trunk/lib"
                        ABSOLUTE BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
    if (NOT EXISTS ${LIB_DIR})
        file(MAKE_DIRECTORY ${LIB_DIR})
    endif()
    add_custom_command(TARGET chardet POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:chardet> ${LIB_DIR})

endif()

install(TARGETS chardet
	RUNTIME
        DESTINATION ${CMAKE_BINARY_DIR}/lib
    LIBRARY
        DESTINATION ${CMAKE_BINARY_DIR}/lib
    ARCHIVE
        DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(FILES
        ${CHARDET_DIR}/src/chardet.h
        ${CHARDET_DIR}/include/version.h
    DESTINATION ${CMAKE_BINARY_DIR}/include/chardet)

set(MAIN_HEADER "${CMAKE_BINARY_DIR}/include/chardet/chardet.h")

# angle-bracket include breaks the installed header, which sits next to version.h
install(CODE "
    file(READ \"${MAIN_HEADER}\" MAIN_HEADER_CONTENT)
    string(REPLACE \"#include <version.h>\" \"#include \\\"version.h\\\"\" MAIN_HEADER_CONTENT \"\${MAIN_HEADER_CONTENT}\")
    file(WRITE \"${MAIN_HEADER}\" \"\${MAIN_HEADER_CONTENT}\")
")
