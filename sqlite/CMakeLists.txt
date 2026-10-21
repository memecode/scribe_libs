cmake_minimum_required(VERSION 3.20)
project(scribeSqlite LANGUAGES C)

set(SQLITE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/v3.54.1")

find_package(Threads REQUIRED)

add_library(sqlite3 STATIC "${SQLITE_DIR}/sqlite3.c")
target_include_directories(sqlite3 PUBLIC "${SQLITE_DIR}")
target_link_libraries(sqlite3 PUBLIC Threads::Threads ${CMAKE_DL_LIBS})

if(UNIX)
    target_link_libraries(sqlite3 PUBLIC m)
endif()

install(TARGETS sqlite3 ARCHIVE DESTINATION "${CMAKE_BINARY_DIR}/lib")
install(FILES "${SQLITE_DIR}/sqlite3.h" "${SQLITE_DIR}/sqlite3ext.h"
    DESTINATION "${CMAKE_BINARY_DIR}/include")