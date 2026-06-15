cmake_minimum_required(VERSION 4.3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(zstring LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Werror=return-type>
    -fno-rtti
    -fno-exceptions
    -fpermissive
    -Wall
    -Wextra
    -nostdinc++
)
add_link_options(
    -nostdlib++
)

set(exe_name ${PROJECT_NAME})
add_executable(${exe_name} src/main.cpp)

add_subdirectory(tests)
