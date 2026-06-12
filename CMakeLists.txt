cmake_minimum_required(VERSION 4.3.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# https://github.com/Kitware/CMake/blob/v4.3.0/Help/dev/experimental.rst#c-import-std-support
# set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD 451f2fe2-a8a2-47c3-bc32-94786d8fc91b)
# set(CMAKE_CXX_MODULE_STD ON)

project(nzd_string LANGUAGES CXX)

set(CMAKE_CXX_EXTENSIONS OFF)
add_compile_options(
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Werror=return-type>
)

set(exe_name ${PROJECT_NAME})
add_executable(${exe_name})
target_sources(${exe_name} PRIVATE
    src/main.cpp
)
target_compile_features(${exe_name} PRIVATE cxx_std_23)
target_compile_options(${exe_name} PRIVATE
    -fno-rtti
    -fno-exceptions
    -fpermissive
    -Wall
    -Wextra
    -nostdinc++
)
target_link_options(${exe_name} PRIVATE
    -nostdlib++
)
