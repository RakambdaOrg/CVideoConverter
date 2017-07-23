cmake_minimum_required(VERSION 3.8)
project(VideoNormalizer)

set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-I/usr/local/include/libavformat -I/usr/local/include/libavcodec -I/usr/local/include/libavutil -L/usr/local/lib -lavformat -lavcodec -lavutil")

set(SOURCE_FILES main.c)
add_executable(VideoNormalizer ${SOURCE_FILES})