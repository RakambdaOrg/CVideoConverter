cmake_minimum_required(VERSION 3.7)
project(VideoNormalizer)

set(CMAKE_CXX_STANDARD 14)
set(LOCAL_LIBS_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR})

find_library(LIBAV_UTIL_LIBRARY NAMES libavutil avutil HINTS ${LOCAL_LIBS_PREFIX_PATH}/lib/windows/ffmpeg)
find_library(LIBAV_FORMAT_LIBRARY NAMES libavformat avformat HINTS ${LOCAL_LIBS_PREFIX_PATH}/lib/windows/ffmpeg)
find_library(LIBAV_CODEC_LIBRARY NAMES libavcodec avcodec HINTS ${LOCAL_LIBS_PREFIX_PATH}/lib/windows/ffmpeg)
find_library(SQLITE_LIBRARY NAMES sqlite3)

IF (WIN32)
    include_directories(include/windows/ffmpeg)
ELSE ()
    include_directories(/usr/local/include)
ENDIF ()

file(GLOB base_SRC
        "src/headers/*.h"
        "src/*.c"
        )
set(SOURCE_FILES ${base_SRC})

message(STATUS ${SOURCE_FILES})

add_executable(VideoNormalizer "")
target_sources(VideoNormalizer
        PUBLIC
        src/Database.cpp
        src/Main.cpp
        src/Processor.cpp

        src/headers/Database.h
        src/headers/IOException.h
        src/headers/NotUsedException.h
        src/headers/Processor.h
        src/headers/VInfos.h
        )


target_link_libraries(VideoNormalizer PUBLIC ${LIBAV_UTIL_LIBRARY} ${LIBAV_FORMAT_LIBRARY} ${LIBAV_CODEC_LIBRARY} ${SQLITE_LIBRARY} -static)
