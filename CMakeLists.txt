cmake_minimum_required(VERSION 3.8)
project(VideoNormalizer)

set(CMAKE_CXX_STANDARD 14)

IF (WIN32)
    include_directories(D:\\Programmes\\MSYS2\\usr\\local\\include)
    link_directories(D:\\Programmes\\MSYS2\\usr\\local\\lib)
ELSE ()
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
ENDIF ()

set(SOURCE_FILES Main.cpp Database.cpp Database.h VInfos.h IOException.h Processor.cpp Processor.h NotUsedException.cpp NotUsedException.h)
add_executable(VideoNormalizer ${SOURCE_FILES})

IF (WIN32)
    target_link_libraries(VideoNormalizer libavutil libavformat libavcodec sqlite3)
ELSE ()
    target_link_libraries(VideoNormalizer avutil avformat avcodec sqlite3)
ENDIF ()