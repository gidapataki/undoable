cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


# Undoable

file(GLOB undoable_srcs
    src/*.cpp
)

file(GLOB undoable_test_srcs
    test/*.cpp
)

add_library(undoable
    ${undoable_srcs}
)

target_include_directories(undoable PUBLIC include)
target_include_directories(undoable PRIVATE src)


# Tests

add_library(test-utils-main test-utils/main.cpp)
target_include_directories(test-utils-main PUBLIC test-utils)

add_executable(test-undoable
    ${undoable_test_srcs}
)

target_link_libraries(test-undoable
    PUBLIC undoable test-utils-main
)

target_include_directories(test-undoable
    PUBLIC test
)
