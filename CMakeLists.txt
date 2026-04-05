cmake_minimum_required(VERSION 3.10)

if(WIN32)
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()

project(NanoLog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(NANOLOG_BUILD_EXPERIMENTAL_TESTS "Build experimental GoogleTest-based runtime tests" OFF)

if(NANOLOG_BUILD_EXPERIMENTAL_TESTS)
    enable_testing()
endif()

add_subdirectory(runtime)
add_subdirectory(sample)
add_subdirectory(sample_preprocessor)
