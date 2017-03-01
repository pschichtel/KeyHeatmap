cmake_minimum_required(VERSION 3.6)
project(KeyHeatmap)

set(CMAKE_C_STANDARD 99)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

list(APPEND CMAKE_REQUIRED_LIBRARIES m)

set(SOURCE_FILES main.c util.c)
add_executable(KeyHeatmap ${SOURCE_FILES})
target_link_libraries(KeyHeatmap Threads::Threads)
target_link_libraries(KeyHeatmap m)
