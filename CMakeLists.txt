cmake_minimum_required(VERSION 3.7)
project(VideoNormalizer)

set(CMAKE_CXX_STANDARD 14)

find_library(LIBAV_UTIL_LIBRARY NAMES libavutil avutil HINTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/ffmpeg)
find_library(LIBAV_FORMAT_LIBRARY NAMES libavformat avformat HINTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/ffmpeg)
find_library(LIBAV_CODEC_LIBRARY NAMES libavcodec avcodec HINTS ${CMAKE_CURRENT_SOURCE_DIR}/lib/windows/ffmpeg)
find_library(SQLITE_LIBRARY NAMES sqlite3)

IF (WIN32)
    include_directories(include/windows/ffmpeg)
ELSE ()
    include_directories(/usr/local/include)
ENDIF ()

add_executable(VideoNormalizer "")
target_sources(VideoNormalizer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Database.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/src/Processor.cpp

        ${CMAKE_CURRENT_SOURCE_DIR}/src/headers/Database.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/headers/IOException.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/headers/NotUsedException.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/headers/Processor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/headers/VInfos.h
        )

target_link_libraries(VideoNormalizer PUBLIC ${LIBAV_UTIL_LIBRARY} ${LIBAV_FORMAT_LIBRARY} ${LIBAV_CODEC_LIBRARY} ${SQLITE_LIBRARY})
