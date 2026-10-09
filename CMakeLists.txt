cmake_minimum_required(VERSION 3.20)
project(naturalizr VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


add_library(naturalizr_core STATIC src/rules.cpp)
target_include_directories(naturalizr_core PUBLIC include)
target_compile_options(naturalizr_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(naturalizr src/main.cpp)
target_link_libraries(naturalizr PRIVATE naturalizr_core)