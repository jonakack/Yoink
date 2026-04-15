set(CMAKE_PREFIX_PATH "C:/Qt/6.10.1/mingw_64")
set(CMAKE_C_COMPILER   "C:/Qt/Tools/mingw1310_64/bin/gcc.exe")
set(CMAKE_CXX_COMPILER "C:/Qt/Tools/mingw1310_64/bin/g++.exe")

cmake_minimum_required(VERSION 3.20)

project(Yoink)

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)

set(CMAKE_AUTOMOC ON)

add_executable(Yoink WIN32 
    src/main.cpp
    src/mainwindow.cpp
    src/downloader.cpp
    src/extractor.cpp
    src/resources.rc)

target_link_libraries(Yoink PRIVATE Qt6::Widgets Qt6::Concurrent)

# yt-dlp -U