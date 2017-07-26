cmake_minimum_required(VERSION 3.8)
project(VideoNormalizer)

set(CMAKE_C_STANDARD 11)

link_directories(/usr/local/lib D:\\Programmes\\MSYS2\\mingw64\\lib)

set(SOURCE_FILES main.c main.h)
add_executable(VideoNormalizer ${SOURCE_FILES})
target_link_libraries(VideoNormalizer libavutil libavformat libavcodec)