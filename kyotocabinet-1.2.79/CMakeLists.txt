project(kyotocabinet)
cmake_minimum_required(VERSION 3.6)

set(source
  kccompare.cc
  kccompress.cc
  kcdb.cc
  kcfile.cc
  kclangc.cc
  kcpolydb.cc
  kcprotodb.cc
  kcregex.cc
  kcstashdb.cc
  kctextdb.cc
  kcthread.cc
  kctreemgr.cc
  kcutil.cc)

set(TARGET kyotocabinet)
# message(STATUS "source=${source}")

add_library(${TARGET} STATIC ${source})

target_include_directories(${TARGET} PRIVATE .)
if (MSVC)
  target_compile_options(${TARGET} PRIVATE /W1)
else()
  # target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic)
endif()