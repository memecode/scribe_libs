project(terminator)
cmake_minimum_required(VERSION 3.6)

set(KYOTO_PATH ../kyotocabinet-1.2.79)
add_subdirectory(${KYOTO_PATH} kyoto)

file(GLOB source
     src/*.cc)

# message(STATUS "source=${source}")

set(TARGET terminator)
add_library(${TARGET} STATIC ${source})

target_include_directories(${TARGET} PRIVATE ${KYOTO_PATH})
target_link_libraries(${TARGET} kyotocabinet)
if (MSVC)
  target_compile_options(${TARGET} PRIVATE /W2)
else()
  # target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic)
endif()


set(TARGET demo)
add_executable(${TARGET} demo/main.cc)
target_include_directories(${TARGET} PRIVATE ${KYOTO_PATH})
target_link_libraries(${TARGET} terminator kyotocabinet)
