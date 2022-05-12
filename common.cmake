

# message(STATUS "CMAKE_GENERATOR_PLATFORM=${CMAKE_GENERATOR_PLATFORM}")
string(FIND ${CMAKE_GENERATOR_PLATFORM} "64" HAS_WIN64)
if (${HAS_WIN64} GREATER 0)
    set(BITSIZE "64")
else()
    set(BITSIZE "32")
endif()

string(FIND ${CMAKE_GENERATOR} "2015" HAS_VS2015)
if (${HAS_VS2015} GREATER 0)
    set(VSVER "14")
    set(VSYEAR "15")
endif()

string(FIND ${CMAKE_GENERATOR} "2017" HAS_VS2017)
if (${HAS_VS2017} GREATER 0)
    set(VSVER "15")
    set(VSYEAR "17")
endif()

string(FIND ${CMAKE_GENERATOR} "2019" HAS_VS2019)
if (${HAS_VS2019} GREATER 0)
    set(VSVER "16")
    set(VSYEAR "19")
endif()
