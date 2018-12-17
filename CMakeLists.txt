cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB undoable_srcs
    src/*.cpp
)

add_executable(test-undoable
    test/test.cpp
    ${undoable_srcs}
)

target_include_directories(test-undoable
    PUBLIC include
)
