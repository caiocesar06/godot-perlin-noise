cmake_minimum_required(VERSION 3.16)
project(PerlinNoiseProject)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(godot-cpp)

set(SOURCES
    src/register_types.cpp
    src/perlin_noise_gd.cpp
)

add_library(perlin_noise SHARED ${SOURCES})

target_link_libraries(perlin_noise PRIVATE godot-cpp)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(LIB_PREFIX "")
    set(LIB_SUFFIX ".dll")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++")
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(LIB_PREFIX "lib")
    set(LIB_SUFFIX ".so")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(LIB_PREFIX "lib")
    set(LIB_SUFFIX ".dylib")
endif()

set_target_properties(perlin_noise PROPERTIES
    PREFIX "${LIB_PREFIX}"
    SUFFIX "${LIB_SUFFIX}"

    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/project/bin"

    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/project/bin"
)
