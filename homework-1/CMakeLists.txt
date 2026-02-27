cmake_minimum_required(VERSION 3.16)

project(TaskScheduler LANGUAGES C CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

add_library(scheduler STATIC
    scheduler.cpp
    scheduler.h
)

target_include_directories(scheduler PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(app
    main.c
)

target_link_libraries(app PRIVATE scheduler)

set_target_properties(app PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
)