cmake_minimum_required(VERSION 3.16)

project(Matrix LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Исполняемый файл
add_executable(matrix
    main.cpp
    matrix.h
)