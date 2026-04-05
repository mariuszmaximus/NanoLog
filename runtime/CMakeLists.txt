set(NANOLOG_RUNTIME_SOURCES
    Cycles.cc
    GeneratedCodeStub.cc
    Util.cc
    Log.cc
    NanoLog.cc
    RuntimeLogger.cc
    TimeTrace.cc
)

option(NANOLOG_BUILD_EXPERIMENTAL_TESTS "Build experimental GoogleTest-based runtime tests" OFF)

add_library(NanoLog STATIC ${NANOLOG_RUNTIME_SOURCES})

target_include_directories(NanoLog
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MSVC)
    target_compile_options(NanoLog PRIVATE /W4)
elseif(WIN32)
    target_compile_options(NanoLog PRIVATE -Wall -Wextra)
else()
    target_compile_options(NanoLog PRIVATE -Wall -Wextra -Wformat -Werror=format)
endif()

if(NOT WIN32)
    find_package(Threads REQUIRED)
    target_link_libraries(NanoLog PUBLIC Threads::Threads)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(NanoLog PUBLIC rt)
endif()

add_executable(decompressor
    Cycles.cc
    GeneratedCodeStub.cc
    Util.cc
    Log.cc
    LogDecompressor.cc
)

target_include_directories(decompressor
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if(MSVC)
    target_compile_options(decompressor PRIVATE /W4)
elseif(WIN32)
    target_compile_options(decompressor PRIVATE -Wall -Wextra)
else()
    target_compile_options(decompressor PRIVATE -Wall -Wextra)
endif()

if(NOT WIN32)
    target_link_libraries(decompressor
        PRIVATE
            Threads::Threads
    )
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(decompressor PRIVATE rt)
endif()

if(NANOLOG_BUILD_EXPERIMENTAL_TESTS)
    if(EXISTS "${PROJECT_SOURCE_DIR}/googletest/CMakeLists.txt")
        enable_testing()
        set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
        add_subdirectory(${PROJECT_SOURCE_DIR}/googletest
                         ${CMAKE_BINARY_DIR}/googletest
                         EXCLUDE_FROM_ALL)

        add_executable(PackerTest
            PackerTest.cc
        )

        target_include_directories(PackerTest
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}
        )

        if(MSVC)
            target_compile_options(PackerTest PRIVATE /W4)
        elseif(WIN32)
            target_compile_options(PackerTest PRIVATE -Wall -Wextra)
        else()
            target_compile_options(PackerTest PRIVATE -Wall -Wextra)
        endif()

        target_link_libraries(PackerTest
            PRIVATE
                gtest
                gtest_main
        )

        add_test(NAME PackerTest COMMAND PackerTest)

        find_package(Python3 REQUIRED COMPONENTS Interpreter)

        set(NANOLOG_TESTHELPER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/testHelper)
        set(NANOLOG_TESTHELPER_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/testHelper)
        set(NANOLOG_TESTHELPER_PREPROCESSED ${NANOLOG_TESTHELPER_BUILD_DIR}/client.cc.i)
        set(NANOLOG_TESTHELPER_INJECTED ${NANOLOG_TESTHELPER_PREPROCESSED}i)
        set(NANOLOG_TESTHELPER_MAP ${NANOLOG_TESTHELPER_BUILD_DIR}/client.map)
        set(NANOLOG_TESTHELPER_GENERATED ${NANOLOG_TESTHELPER_BUILD_DIR}/GeneratedCode.cc)

        file(MAKE_DIRECTORY ${NANOLOG_TESTHELPER_BUILD_DIR})

        add_custom_command(
            OUTPUT ${NANOLOG_TESTHELPER_PREPROCESSED}
            COMMAND ${CMAKE_COMMAND} -E make_directory ${NANOLOG_TESTHELPER_BUILD_DIR}
            COMMAND ${CMAKE_CXX_COMPILER}
                    -E
                    -I .
                    testHelper/client.cc
                    -o ${NANOLOG_TESTHELPER_PREPROCESSED}
                    -std=c++11
            DEPENDS ${NANOLOG_TESTHELPER_DIR}/client.cc
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            VERBATIM
        )

        add_custom_command(
            OUTPUT ${NANOLOG_TESTHELPER_INJECTED} ${NANOLOG_TESTHELPER_MAP}
            COMMAND ${Python3_EXECUTABLE}
                    ${PROJECT_SOURCE_DIR}/preprocessor/parser.py
                    --mapOutput=${NANOLOG_TESTHELPER_MAP}
                    ${NANOLOG_TESTHELPER_PREPROCESSED}
            DEPENDS
                ${NANOLOG_TESTHELPER_PREPROCESSED}
                ${PROJECT_SOURCE_DIR}/preprocessor/parser.py
                ${PROJECT_SOURCE_DIR}/preprocessor/FunctionGenerator.py
                ${PROJECT_SOURCE_DIR}/preprocessor/docopt.py
            VERBATIM
        )

        add_custom_command(
            OUTPUT ${NANOLOG_TESTHELPER_GENERATED}
            COMMAND ${Python3_EXECUTABLE}
                    ${PROJECT_SOURCE_DIR}/preprocessor/parser.py
                    --combinedOutput=${NANOLOG_TESTHELPER_GENERATED}
                    ${NANOLOG_TESTHELPER_MAP}
            DEPENDS
                ${NANOLOG_TESTHELPER_MAP}
                ${PROJECT_SOURCE_DIR}/preprocessor/parser.py
                ${PROJECT_SOURCE_DIR}/preprocessor/FunctionGenerator.py
                ${PROJECT_SOURCE_DIR}/preprocessor/docopt.py
            VERBATIM
        )

        set(NANOLOG_TEST_RUNTIME_SOURCES
            Cycles.cc
            Util.cc
            Log.cc
            NanoLog.cc
            RuntimeLogger.cc
            TimeTrace.cc
        )

        add_executable(RuntimeTest
            LogTest.cc
            NanoLogTest.cc
            NanoLogCpp17Test.cc
            ${NANOLOG_TEST_RUNTIME_SOURCES}
            ${NANOLOG_TESTHELPER_GENERATED}
        )

        target_compile_definitions(RuntimeTest PRIVATE PREPROCESSOR_NANOLOG)

        target_include_directories(RuntimeTest
            PRIVATE
                ${CMAKE_CURRENT_SOURCE_DIR}
                ${NANOLOG_TESTHELPER_BUILD_DIR}
        )

        if(MSVC)
            target_compile_options(RuntimeTest PRIVATE /W4)
        elseif(WIN32)
            target_compile_options(RuntimeTest PRIVATE -Wall -Wextra)
        else()
            target_compile_options(RuntimeTest PRIVATE -Wall -Wextra -Wno-unused-parameter)
        endif()

        target_link_libraries(RuntimeTest
            PRIVATE
                gtest
                gtest_main
        )

        if(NOT WIN32)
            target_link_libraries(RuntimeTest PRIVATE Threads::Threads)
        endif()

        if(UNIX AND NOT APPLE)
            target_link_libraries(RuntimeTest PRIVATE rt)
        endif()

        add_test(NAME RuntimeTest COMMAND RuntimeTest)
    else()
        message(STATUS "NANOLOG_BUILD_EXPERIMENTAL_TESTS=ON, but googletest submodule is missing; skipping PackerTest")
    endif()
endif()
