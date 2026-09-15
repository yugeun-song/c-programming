# OS-agnostic build script running on CMake itself
cmake_minimum_required(VERSION 3.21)

get_filename_component(ROOT_DIR ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
if(CMAKE_HOST_WIN32)
    set(DEFAULT_TARGET x86_64-windows-msvc)
else()
    cmake_host_system_information(RESULT host_arch QUERY OS_PLATFORM)
    set(DEFAULT_TARGET ${host_arch}-linux-gcc)
endif()
set(TARGET ${DEFAULT_TARGET})
set(BUILD_TYPE Debug)
set(CLEAN FALSE)

execute_process(
    COMMAND ${CMAKE_COMMAND} --list-presets
    WORKING_DIRECTORY ${ROOT_DIR}
    OUTPUT_VARIABLE presets
    ERROR_QUIET
)
string(REGEX MATCHALL "\"[^\"]+\"" presets "${presets}")
list(TRANSFORM presets REPLACE "\"(.+)\"" "  \\1")
list(JOIN presets "\n" presets)
set(usage [=[
usage: cmake -P build.cmake [--] [clean] [debug|release] [target]

Configure build/<target> from its CMake preset and build every program into bin/.

  clean       delete build/<target> first
  debug       Debug configuration (default)
  release     Release configuration
  target      configure preset to use (default @DEFAULT_TARGET@)
  -h, --help  print this help (after --)

Arguments are case-insensitive and may appear in any order.

Targets on this host:
@presets@]=])
string(CONFIGURE "${usage}" usage @ONLY)

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
        elseif(word STREQUAL "-h" OR word STREQUAL "--help")
            execute_process(COMMAND ${CMAKE_COMMAND} -E echo "${usage}")
            return()
        elseif(word MATCHES "^[a-z0-9_]+-")
            set(TARGET ${word})
        else()
            message("${usage}")
            message(FATAL_ERROR "unknown argument: ${CMAKE_ARGV${i}}")
        endif()
    endif()
endforeach()

set(BUILD_DIR "${ROOT_DIR}/build/${TARGET}")

message(STATUS "-------------------------------------------------")
message(STATUS "Build Script Started")
message(STATUS "Root:   ${ROOT_DIR}")
message(STATUS "Target: ${TARGET} (${BUILD_TYPE})")
message(STATUS "-------------------------------------------------")

if(CLEAN)
    message(STATUS ">> Cleaning ${BUILD_DIR}...")
    file(REMOVE_RECURSE "${BUILD_DIR}")
endif()

message(STATUS "[Step 1] Configuring...")
execute_process(
    COMMAND ${CMAKE_COMMAND} --preset ${TARGET}
    WORKING_DIRECTORY ${ROOT_DIR}
    RESULT_VARIABLE result
)

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