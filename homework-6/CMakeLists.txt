cmake_minimum_required(VERSION 3.14)
project(homework_6_cycle_detection LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(homework_6
    main.cpp
)

add_executable(homework_6_generator
    generator.cpp
)
