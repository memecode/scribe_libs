project(libchardet)
cmake_minimum_required(VERSION 3.8)

include(../common.cmake)
if (WIN32)
    set(TARGET "chardet${VSVER}x${BITSIZE}")
else()
    set(TARGET "chardet")
endif()

add_library(${TARGET} SHARED
    src/chardet.cpp
    src/CharDistribution.cpp
    src/JpCntx.cpp
    src/nsBig5Prober.cpp
    src/nsCharSetProber.cpp
    src/nsEscCharsetProber.cpp
    src/nsEscSM.cpp
    src/nsEUCJPProber.cpp
    src/nsEUCKRProber.cpp
    src/nsEUCTWProber.cpp
    src/nsGB2312Prober.cpp
    src/nsHebrewProber.cpp
    src/nsLatin1Prober.cpp
    src/nsMBCSGroupProber.cpp
    src/nsMBCSSM.cpp
    src/nsSBCharSetProber.cpp
    src/nsSBCSGroupProber.cpp
    src/nsSJISProber.cpp
    src/nsUniversalDetector.cpp
    src/nsUTF8Prober.cpp
    src/tables/LangCyrillicModel.cpp
    src/tables/LangGreekModel.cpp
    src/tables/LangBulgarianModel.cpp
    src/tables/LangHungarianModel.cpp
    src/tables/LangThaiModel.cpp
    src/tables/LangHebrewModel.cpp
    src/tables/LangArabicModel.cpp
    src/tables/LangDanishModel.cpp
    src/tables/LangEsperantoModel.cpp
    src/tables/LangFrenchModel.cpp
    src/tables/LangGermanModel.cpp
    src/tables/LangSpanishModel.cpp
    src/tables/LangTurkishModel.cpp
    src/tables/LangVietnameseModel.cpp
    )

target_include_directories(${TARGET} PUBLIC include src)
target_compile_definitions(${TARGET} PRIVATE -DHAVE_DLL_EXPORT)

set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13")

if (WIN32)
    get_filename_component(LIB_DIR "../../../Lgi/trunk/lib"
                        ABSOLUTE BASE_DIR ${CMAKE_CURRENT_LIST_DIR})
    add_custom_command(TARGET ${TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET}> ${LIB_DIR})
endif()
