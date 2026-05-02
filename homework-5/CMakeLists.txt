cmake_minimum_required(VERSION 3.15)

project(hw_5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(hw_5
    main.cpp
)

target_include_directories(hw_5 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

