# OS-agnostic build script running on CMake itself
cmake_minimum_required(VERSION 3.12)

get_filename_component(ROOT_DIR ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
set(BUILD_DIR "${ROOT_DIR}/build")
set(BUILD_TYPE Debug)
set(CLEAN FALSE)

math(EXPR last_arg "${CMAKE_ARGC} - 1")
foreach(i RANGE ${last_arg})
    if(CMAKE_ARGV${i} STREQUAL "-P")
        math(EXPR first_word "${i} + 2")
    elseif(DEFINED first_word AND i GREATER_EQUAL first_word AND NOT CMAKE_ARGV${i} STREQUAL "--")
        string(TOLOWER "${CMAKE_ARGV${i}}" word)
        if(word STREQUAL "clean")
            set(CLEAN TRUE)
        elseif(word STREQUAL "debug")
            set(BUILD_TYPE Debug)
        elseif(word STREQUAL "release")
            set(BUILD_TYPE Release)
        else()
            message(FATAL_ERROR "usage: cmake -P build.cmake [clean] [debug|release]")
        endif()
    endif()
endforeach()

message(STATUS "-------------------------------------------------")
message(STATUS "Build Script Started")
message(STATUS "Root:  ${ROOT_DIR}")
message(STATUS "Build: ${BUILD_DIR} (${BUILD_TYPE})")
message(STATUS "-------------------------------------------------")

message(STATUS "[Step 1] Configuring...")

# This script forces a generator, so a cache left by a different one has to go first
if(CLEAN OR EXISTS "${BUILD_DIR}/CMakeCache.txt")
    message(STATUS ">> Cleaning previous build...")
    file(REMOVE_RECURSE "${BUILD_DIR}")
endif()

# Determine the generator based on OS
if(WIN32)
    # Force Visual Studio to prevent CMake from accidentally picking MinGW/Ninja
    message(STATUS ">> Windows detected: Probing for Visual Studio (2026 down to 2019)...")

    # Newest first: VS 2026 (v18), VS 2022 (v17), VS 2019 (v16)
    set(vs_years 2026 2022 2019)
    set(vs_majors 18 17 16)

    set(found_generator FALSE)
    list(LENGTH vs_years len)
    math(EXPR range "${len} - 1")

    foreach(i RANGE ${range})
        list(GET vs_years ${i} year)
        list(GET vs_majors ${i} major)
        set(current_gen "Visual Studio ${major} ${year}")
        file(REMOVE_RECURSE "${BUILD_DIR}")

        execute_process(
            COMMAND ${CMAKE_COMMAND} -G "${current_gen}" -A x64 -S ${ROOT_DIR} -B ${BUILD_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
            RESULT_VARIABLE result
            OUTPUT_QUIET
            ERROR_QUIET
        )

        if(result EQUAL 0)
            message(STATUS ">> Successfully configured with ${current_gen}")
            set(found_generator TRUE)
            break()
        endif()
    endforeach()

    if(NOT found_generator)
        message(FATAL_ERROR "MSVC not found! Please install Visual Studio 2019 or later.")
    endif()
else()
    # On Linux/Unix: Use Unix Makefiles
    message(STATUS ">> Linux/Unix detected: Using Unix Makefiles...")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -G "Unix Makefiles" -S ${ROOT_DIR} -B ${BUILD_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
        RESULT_VARIABLE result
    )
endif()

if(NOT result EQUAL 0)
    message(FATAL_ERROR "Configuration failed!")
endif()

message(STATUS "\n[Step 2] Building...")
execute_process(
    COMMAND ${CMAKE_COMMAND} --build ${BUILD_DIR} --parallel --config ${BUILD_TYPE}
    RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "Build failed!")
endif()

message(STATUS "\n[Success] Binaries are in 'bin/' directory.")