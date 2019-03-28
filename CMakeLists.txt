cmake_minimum_required(VERSION 3.7)
project(VideoNormalizer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_library(LIBAV_UTIL_LIBRARY NAMES libavutil avutil HINTS ${CMAKE_PREFIX_PATH}/lib/windows/ffmpeg)
find_library(LIBAV_FORMAT_LIBRARY NAMES libavformat avformat HINTS ${CMAKE_PREFIX_PATH}/lib/windows/ffmpeg)
find_library(LIBAV_CODEC_LIBRARY NAMES libavcodec avcodec HINTS ${CMAKE_PREFIX_PATH}/lib/windows/ffmpeg)
find_library(SQLITE_LIBRARY NAMES sqlite3)

IF (WIN32)
    include_directories(include/windows/libav)
ELSE ()
    include_directories(/usr/local/include)
ENDIF ()

set(SOURCE_FILES Main.cpp Database.cpp Database.h Processor.cpp Processor.h VInfos.h IOException.h NotUsedException.cpp NotUsedException.h)
add_executable(VideoNormalizer ${SOURCE_FILES})

target_link_libraries(VideoNormalizer ${LIBAV_UTIL_LIBRARY} ${LIBAV_FORMAT_LIBRARY} ${LIBAV_CODEC_LIBRARY} ${SQLITE_LIBRARY})
