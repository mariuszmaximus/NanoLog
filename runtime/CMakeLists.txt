set(NANOLOG_RUNTIME_SOURCES
    Cycles.cc
    GeneratedCodeStub.cc
    Util.cc
    Log.cc
    NanoLog.cc
    RuntimeLogger.cc
    TimeTrace.cc
)

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
