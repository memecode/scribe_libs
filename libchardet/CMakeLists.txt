project(libchardet)
cmake_minimum_required(VERSION 3.8)

add_library(chardet SHARED
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

target_include_directories(chardet PUBLIC include src)
target_compile_definitions(chardet PRIVATE -DHAVE_DLL_EXPORT)