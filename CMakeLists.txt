cmake_minimum_required(VERSION 3.18)
project(c-programming C)

get_property(IS_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(IS_MULTI_CONFIG)
    set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE STRING "" FORCE)
    set(CMAKE_DEFAULT_BUILD_TYPE Debug)
elseif(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()
set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release)

if(NOT CMAKE_SYSTEM_NAME MATCHES "^(Linux|Windows)$")
    message(FATAL_ERROR "Unsupported target OS '${CMAKE_SYSTEM_NAME}'. This project targets Linux and Windows.")
endif()

# 1. Compiler Options
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/utf-8 /W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    message(STATUS ">> Platform: Windows (MSVC)")
else()
    add_compile_options(-Wall -Wextra -pedantic)
    set(TRACE_C_FLAGS -ggdb3 -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fasynchronous-unwind-tables -pg)
    set(OPT_C_FLAGS -O2)
    message(STATUS ">> Platform: Linux/Unix (GCC/Clang)")
endif()

# 2. Environment Probes
include(CheckCSourceCompiles)
include(CheckSymbolExists)

check_c_source_compiles("
#if !defined(__GNUC__)
#error not the GNU dialect
#endif
int main(void) { return 0; }
" HAVE_GNU_DIALECT)

if(HAVE_GNU_DIALECT)
    set(C_DIALECT gnu)
elseif(MSVC)
    set(C_DIALECT msvc)
else()
    set(C_DIALECT unknown)
endif()

function(probe_arch name predicate)
    check_c_source_compiles("
#if !(${predicate})
#error architecture mismatch
#endif
int main(void) { return 0; }
" ARCH_IS_${name})
    if(ARCH_IS_${name})
        set(TARGET_ARCH ${name} PARENT_SCOPE)
    endif()
endfunction()

set(TARGET_ARCH unknown)
probe_arch(x86_64 "defined(__x86_64__) || defined(_M_X64)")
probe_arch(aarch64 "defined(__aarch64__) || defined(_M_ARM64)")
probe_arch(riscv64 "defined(__riscv) && (__riscv_xlen == 64)")

if(TARGET_ARCH STREQUAL "unknown")
    message(FATAL_ERROR "Unsupported target architecture. This project targets x86_64, aarch64 and riscv64.")
endif()

string(TOLOWER "${CMAKE_SYSTEM_NAME}" TARGET_OS)

string(TOLOWER "${CMAKE_C_COMPILER_ID}" TARGET_CC)
if(TARGET_CC STREQUAL "gnu")
    set(TARGET_CC gcc)
endif()

set(BUILD_TAG "${TARGET_ARCH}-${TARGET_OS}-${TARGET_CC}-$<LOWER_CASE:$<CONFIG>>")
if(IS_MULTI_CONFIG)
    set(BUILD_TAG_SHOWN "${TARGET_ARCH}-${TARGET_OS}-${TARGET_CC}-<config>")
else()
    string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type_lower)
    set(BUILD_TAG_SHOWN "${TARGET_ARCH}-${TARGET_OS}-${TARGET_CC}-${build_type_lower}")
endif()

set(CMAKE_REQUIRED_DEFINITIONS -D_POSIX_C_SOURCE=199309L)
check_symbol_exists(clock_gettime "time.h" HAVE_CLOCK_GETTIME)
unset(CMAKE_REQUIRED_DEFINITIONS)

set(HAVE_GLIBC_LINUX FALSE)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    check_symbol_exists(__GLIBC__ "features.h" HAVE_GLIBC)
    if(HAVE_GLIBC)
        set(HAVE_GLIBC_LINUX TRUE)
    endif()
endif()

message(STATUS ">> Target: ${TARGET_ARCH}-${TARGET_OS}-${TARGET_CC} | dialect: ${C_DIALECT} | glibc on Linux: ${HAVE_GLIBC_LINUX}")

# 3. Source Table
set(STUDY_SOURCES "")

function(study_source rel_path)
    cmake_parse_arguments(ARG "" "DIALECT" "WHEN" ${ARGN})

    if(ARG_DIALECT AND NOT ARG_DIALECT STREQUAL "${C_DIALECT}")
        message(STATUS "      - skipped: ${rel_path} (needs ${ARG_DIALECT} dialect)")
        return()
    endif()

    foreach(cond IN LISTS ARG_WHEN)
        if(NOT ${cond})
            message(STATUS "      - skipped: ${rel_path} (needs ${cond})")
            return()
        endif()
    endforeach()

    set(STUDY_SOURCES ${STUDY_SOURCES} "${rel_path}" PARENT_SCOPE)
endfunction()

study_source(memory/struct_padding.c)
study_source(std/hello_world.c)
study_source(type/format_string.c)
study_source(memory/glibc_heap_fragmentation.c WHEN HAVE_GLIBC_LINUX)
study_source(gnu/statement_expression.c DIALECT gnu)
study_source(gnu/likely.c DIALECT gnu WHEN HAVE_CLOCK_GETTIME)

# 4. Target Generation
foreach(rel_path IN LISTS STUDY_SOURCES)
    get_filename_component(dir_name "${rel_path}" DIRECTORY)
    get_filename_component(filename "${rel_path}" NAME_WE)
    string(REPLACE "/" "_" unique_target "${dir_name}_${filename}")
    set(out_dir "${CMAKE_SOURCE_DIR}/bin/${BUILD_TAG}/${dir_name}")

    if(MSVC)
        add_executable(${unique_target} "${CMAKE_SOURCE_DIR}/${rel_path}")
        message(STATUS "      + Target created: ${unique_target} -> bin/${BUILD_TAG_SHOWN}/${dir_name}/${filename}")

        # Configure output paths for binaries
        set_target_properties(${unique_target} PROPERTIES
            OUTPUT_NAME "${filename}"
            FOLDER "${dir_name}"
            RUNTIME_OUTPUT_DIRECTORY "${out_dir}"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${out_dir}"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${out_dir}"
        )

        # Configure PDB paths for MSVC debugging
        set_target_properties(${unique_target} PROPERTIES
            PDB_OUTPUT_DIRECTORY "${out_dir}"
            PDB_OUTPUT_DIRECTORY_DEBUG "${out_dir}"
        )
    else()
        add_executable(${unique_target} "${CMAKE_SOURCE_DIR}/${rel_path}")
        target_compile_options(${unique_target} PRIVATE ${TRACE_C_FLAGS})
        target_link_options(${unique_target} PRIVATE -pg)
        set_target_properties(${unique_target} PROPERTIES
            OUTPUT_NAME "${filename}"
            FOLDER "${dir_name}"
            RUNTIME_OUTPUT_DIRECTORY "${out_dir}"
        )
        message(STATUS "      + Target created: ${unique_target} -> bin/${BUILD_TAG_SHOWN}/${dir_name}/${filename} (trace)")

        add_executable(${unique_target}_opt "${CMAKE_SOURCE_DIR}/${rel_path}")
        target_compile_options(${unique_target}_opt PRIVATE ${OPT_C_FLAGS})
        set_target_properties(${unique_target}_opt PROPERTIES
            OUTPUT_NAME "${filename}_opt"
            FOLDER "${dir_name}"
            RUNTIME_OUTPUT_DIRECTORY "${out_dir}"
        )
        message(STATUS "      + Target created: ${unique_target}_opt -> bin/${BUILD_TAG_SHOWN}/${dir_name}/${filename}_opt (optimized)")
    endif()
endforeach()
