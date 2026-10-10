cmake_minimum_required(VERSION 3.20)
project(Lab-work-2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(Lab-work-2
    main.cpp
    sequenceGenerator.cpp
    sequenceStore.cpp
)

if(MSVC)
    target_compile_options(Lab-work-2 PRIVATE /W4 /utf-8)
else()
    target_compile_options(Lab-work-2 PRIVATE -Wall -Wextra -Wpedantic)
endif()
