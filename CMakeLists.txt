cmake_minimum_required(VERSION 3.20)
project(naturalizr VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(tomlplusplus
  GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
  GIT_TAG        v3.4.0
  GIT_SHALLOW    ON)
FetchContent_MakeAvailable(tomlplusplus)

add_library(naturalizr_core STATIC src/rules.cpp)
target_include_directories(naturalizr_core PUBLIC include)
target_compile_options(naturalizr_core PRIVATE -Wall -Wextra -Wpedantic)

add_executable(naturalizr src/main.cpp)
target_link_libraries(naturalizr PRIVATE naturalizr_core)
target_link_libraries(naturalizr_core PRIVATE tomlplusplus::tomlplusplus)