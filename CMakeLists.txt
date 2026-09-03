cmake_minimum_required(VERSION 3.16)
project(MV26 C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(mv26
    src/main.c
    src/funct.c
    src/logic.c
)

target_include_directories(mv26 PRIVATE include)

if(MSVC)
    target_compile_options(mv26 PRIVATE /W4)
else()
    target_compile_options(mv26 PRIVATE -Wall -Wextra)
endif()