cmake_minimum_required(VERSION 3.6)
project(KeyHeatmap)

if (CMAKE_VERSION VERSION_LESS "3.1")
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(CMAKE_C_FLAGS "--std=gnu99 ${CMAKE_C_FLAGS}")
    endif ()
else ()
    set(CMAKE_C_STANDARD 99)
endif ()


set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

list(APPEND CMAKE_REQUIRED_LIBRARIES m)

set(SOURCE_FILES main.c util.c)
add_executable(KeyHeatmap ${SOURCE_FILES})
target_link_libraries(KeyHeatmap m)

find_package(Threads REQUIRED)
if (THREADS_HAVE_PTHREAD_ARG)
    target_compile_options(KeyHeatmap PUBLIC "-pthread")
endif ()
if (CMAKE_THREAD_LIBS_INIT)
    target_link_libraries(KeyHeatmap "${CMAKE_THREAD_LIBS_INIT}")
endif ()
