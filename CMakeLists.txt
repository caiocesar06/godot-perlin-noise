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

find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    target_link_libraries(perlin_noise PRIVATE OpenMP::OpenMP_CXX)
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(LIB_PREFIX "")
    set(LIB_SUFFIX ".dll")

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static-libgcc -static-libstdc++")

        get_filename_component(MINGW_BIN_DIR ${CMAKE_CXX_COMPILER} DIRECTORY)

        add_custom_command(TARGET perlin_noise POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
                "$<TARGET_FILE_DIR:perlin_noise>"

            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MINGW_BIN_DIR}/libwinpthread-1.dll"
                "$<TARGET_FILE_DIR:perlin_noise>"

            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${MINGW_BIN_DIR}/libgomp-1.dll"
                "$<TARGET_FILE_DIR:perlin_noise>"

            COMMENT "Automação: Copiando dependências do MinGW (libgcc, Pthreads, OpenMP) para project/bin..."
        )
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

    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/project/addons/perlin_noise/bin"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/project/addons/perlin_noise/bin"
)
