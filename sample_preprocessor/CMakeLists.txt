find_package(Python3 REQUIRED COMPONENTS Interpreter)

if(NOT WIN32)
    find_package(Threads REQUIRED)
endif()

set(SAMPLE_PREPROC_RUNTIME_SOURCES
    ${PROJECT_SOURCE_DIR}/runtime/Cycles.cc
    ${PROJECT_SOURCE_DIR}/runtime/NanoLog.cc
    ${PROJECT_SOURCE_DIR}/runtime/Util.cc
    ${PROJECT_SOURCE_DIR}/runtime/Log.cc
    ${PROJECT_SOURCE_DIR}/runtime/RuntimeLogger.cc
    ${PROJECT_SOURCE_DIR}/runtime/TimeTrace.cc
)

set(SAMPLE_PREPROC_RUNTIME_FLAGS
    -std=c++11
    -O3
    -DNDEBUG
    -g
)

set(SAMPLE_PREPROC_APP_FLAGS
    -std=c++17
    -DNDEBUG
    -O3
    -g
)

set(SAMPLE_PREPROC_GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated)
set(SAMPLE_PREPROC_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/main.cc)
set(SAMPLE_PREPROC_PREPROCESSED ${SAMPLE_PREPROC_GENERATED_DIR}/main.cc.i)
set(SAMPLE_PREPROC_INJECTED ${SAMPLE_PREPROC_PREPROCESSED}i)
set(SAMPLE_PREPROC_MAP ${SAMPLE_PREPROC_GENERATED_DIR}/main.cc.map)
set(SAMPLE_PREPROC_GENERATED_CODE ${SAMPLE_PREPROC_GENERATED_DIR}/GeneratedCode.cc)
set(SAMPLE_PREPROC_USER_OBJECT ${SAMPLE_PREPROC_GENERATED_DIR}/main.cc.o)

file(MAKE_DIRECTORY ${SAMPLE_PREPROC_GENERATED_DIR})

add_custom_command(
    OUTPUT ${SAMPLE_PREPROC_PREPROCESSED}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${SAMPLE_PREPROC_GENERATED_DIR}
    COMMAND ${CMAKE_CXX_COMPILER}
            -E
            -I ${PROJECT_SOURCE_DIR}/runtime
            ${SAMPLE_PREPROC_SOURCE}
            -o ${SAMPLE_PREPROC_PREPROCESSED}
            -std=c++11
    DEPENDS ${SAMPLE_PREPROC_SOURCE}
    VERBATIM
)

add_custom_command(
    OUTPUT ${SAMPLE_PREPROC_INJECTED} ${SAMPLE_PREPROC_MAP}
    COMMAND ${Python3_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/preprocessor/parser.py
            --mapOutput=${SAMPLE_PREPROC_MAP}
            ${SAMPLE_PREPROC_PREPROCESSED}
    DEPENDS
        ${SAMPLE_PREPROC_PREPROCESSED}
        ${PROJECT_SOURCE_DIR}/preprocessor/parser.py
        ${PROJECT_SOURCE_DIR}/preprocessor/FunctionGenerator.py
        ${PROJECT_SOURCE_DIR}/preprocessor/docopt.py
    VERBATIM
)

add_custom_command(
    OUTPUT ${SAMPLE_PREPROC_USER_OBJECT}
    COMMAND ${CMAKE_CXX_COMPILER}
            -c
            -o ${SAMPLE_PREPROC_USER_OBJECT}
            ${SAMPLE_PREPROC_INJECTED}
            ${SAMPLE_PREPROC_APP_FLAGS}
    DEPENDS ${SAMPLE_PREPROC_INJECTED}
    VERBATIM
)

add_custom_command(
    OUTPUT ${SAMPLE_PREPROC_GENERATED_CODE}
    COMMAND ${Python3_EXECUTABLE}
            ${PROJECT_SOURCE_DIR}/preprocessor/parser.py
            --combinedOutput=${SAMPLE_PREPROC_GENERATED_CODE}
            ${SAMPLE_PREPROC_MAP}
    DEPENDS
        ${SAMPLE_PREPROC_MAP}
        ${PROJECT_SOURCE_DIR}/preprocessor/parser.py
        ${PROJECT_SOURCE_DIR}/preprocessor/FunctionGenerator.py
        ${PROJECT_SOURCE_DIR}/preprocessor/docopt.py
    VERBATIM
)

add_library(samplePreprocessorNanoLog STATIC
    ${SAMPLE_PREPROC_RUNTIME_SOURCES}
    ${SAMPLE_PREPROC_GENERATED_CODE}
)

target_compile_definitions(samplePreprocessorNanoLog PRIVATE PREPROCESSOR_NANOLOG)

target_include_directories(samplePreprocessorNanoLog
    PUBLIC
        ${PROJECT_SOURCE_DIR}/runtime
    PRIVATE
        ${SAMPLE_PREPROC_GENERATED_DIR}
)

if(MSVC)
    target_compile_options(samplePreprocessorNanoLog PRIVATE /W4)
elseif(WIN32)
    target_compile_options(samplePreprocessorNanoLog PRIVATE ${SAMPLE_PREPROC_RUNTIME_FLAGS} -Wall -Wextra)
else()
    target_compile_options(samplePreprocessorNanoLog PRIVATE
        ${SAMPLE_PREPROC_RUNTIME_FLAGS}
        -Wall
        -Wformat=2
        -Wextra
        -Wwrite-strings
        -Wno-unused-parameter
        -Wmissing-format-attribute
        -Wno-non-template-friend
        -Woverloaded-virtual
        -Wcast-qual
        -Wcast-align
        -Wno-address-of-packed-member
        -Wconversion
        -Weffc++
    )
endif()

if(NOT WIN32)
    target_link_libraries(samplePreprocessorNanoLog PUBLIC Threads::Threads)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(samplePreprocessorNanoLog PUBLIC rt)
endif()

add_executable(samplePreprocessorDecompressor
    ${SAMPLE_PREPROC_RUNTIME_SOURCES}
    ${PROJECT_SOURCE_DIR}/runtime/LogDecompressor.cc
    ${SAMPLE_PREPROC_GENERATED_CODE}
)

set_target_properties(samplePreprocessorDecompressor PROPERTIES OUTPUT_NAME decompressor)

target_compile_definitions(samplePreprocessorDecompressor PRIVATE PREPROCESSOR_NANOLOG)

target_include_directories(samplePreprocessorDecompressor
    PRIVATE
        ${PROJECT_SOURCE_DIR}/runtime
        ${SAMPLE_PREPROC_GENERATED_DIR}
)

if(MSVC)
    target_compile_options(samplePreprocessorDecompressor PRIVATE /W4)
elseif(WIN32)
    target_compile_options(samplePreprocessorDecompressor PRIVATE ${SAMPLE_PREPROC_RUNTIME_FLAGS} -Wall -Wextra)
else()
    target_compile_options(samplePreprocessorDecompressor PRIVATE
        ${SAMPLE_PREPROC_RUNTIME_FLAGS}
        -Wall
        -Wformat=2
        -Wextra
        -Wwrite-strings
        -Wno-unused-parameter
        -Wmissing-format-attribute
        -Wno-non-template-friend
        -Woverloaded-virtual
        -Wcast-qual
        -Wcast-align
        -Wno-address-of-packed-member
        -Wconversion
        -Weffc++
    )
endif()

if(NOT WIN32)
    target_link_libraries(samplePreprocessorDecompressor PRIVATE Threads::Threads)
endif()

if(UNIX AND NOT APPLE)
    target_link_libraries(samplePreprocessorDecompressor PRIVATE rt)
endif()

set_source_files_properties(${SAMPLE_PREPROC_USER_OBJECT}
    PROPERTIES
        EXTERNAL_OBJECT TRUE
        GENERATED TRUE
)

add_executable(samplePreprocessorApplication ${SAMPLE_PREPROC_USER_OBJECT})
set_target_properties(samplePreprocessorApplication PROPERTIES OUTPUT_NAME sampleApplication)

target_link_libraries(samplePreprocessorApplication PRIVATE samplePreprocessorNanoLog)

add_dependencies(samplePreprocessorApplication samplePreprocessorDecompressor)
