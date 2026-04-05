add_executable(sampleApplication main.cc)

target_link_libraries(sampleApplication
    PRIVATE
        NanoLog
)

target_include_directories(sampleApplication
    PRIVATE
        ${PROJECT_SOURCE_DIR}/runtime
)

if(MSVC)
    target_compile_options(sampleApplication PRIVATE /W4)
elseif(WIN32)
    target_compile_options(sampleApplication PRIVATE -DNDEBUG -O3 -g)
else()
    target_compile_options(sampleApplication PRIVATE -Werror=format -DNDEBUG -O3 -g)
endif()
