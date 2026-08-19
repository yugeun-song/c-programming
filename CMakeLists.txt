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

option(ENABLE_LTO "Link-time optimization in Release" OFF)
option(ENABLE_GPROF "Instrument for gprof (-pg)" OFF)
set(SANITIZE "" CACHE STRING "Sanitizers to enable, e.g. address,undefined")

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
    message(STATUS ">> Platform: Linux/Unix (GCC/Clang)")
endif()

# 2. Environment Probes
include(CheckCCompilerFlag)
include(CheckCSourceCompiles)
include(CheckLinkerFlag)
include(CheckPIESupported)
include(CheckSymbolExists)

function(probe_dialect name predicate)
    check_c_source_compiles("
#if !(${predicate})
#error dialect mismatch
#endif
int main(void) { return 0; }
" DIALECT_IS_${name})
    if(DIALECT_IS_${name})
        set(C_DIALECTS ${C_DIALECTS} ${name} PARENT_SCOPE)
    endif()
endfunction()

set(C_DIALECTS "")
probe_dialect(gnu "defined(__GNUC__)")
probe_dialect(gcc "defined(__GNUC__) && !defined(__clang__)")
probe_dialect(clang "defined(__clang__)")
probe_dialect(msvc "defined(_MSC_VER)")

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

if(TARGET_OS STREQUAL "windows" AND NOT TARGET_ARCH STREQUAL "x86_64")
    message(FATAL_ERROR "Unsupported combination: windows on ${TARGET_ARCH}. Windows builds target x86_64 only.")
endif()

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

string(REPLACE ";" "," dialects_shown "${C_DIALECTS}")
message(STATUS ">> Target: ${TARGET_ARCH}-${TARGET_OS}-${TARGET_CC} | dialects: ${dialects_shown} | glibc on Linux: ${HAVE_GLIBC_LINUX}")

# 3. Configuration Flags
function(keep_supported_flags out_var tag)
    set(accepted "")
    foreach(flag IN LISTS ARGN)
        string(MAKE_C_IDENTIFIER "ACCEPTS_${tag}_${flag}" probe)
        check_c_compiler_flag("${flag}" ${probe})
        if(${probe})
            list(APPEND accepted "${flag}")
        endif()
    endforeach()
    set(${out_var} ${accepted} PARENT_SCOPE)
endfunction()

function(keep_supported_link_flags out_var tag)
    set(accepted "")
    foreach(flag IN LISTS ARGN)
        string(MAKE_C_IDENTIFIER "LINKS_${tag}_${flag}" probe)
        check_linker_flag(C "${flag}" ${probe})
        if(${probe})
            list(APPEND accepted "${flag}")
        endif()
    endforeach()
    set(${out_var} ${accepted} PARENT_SCOPE)
endfunction()

if(MSVC)
    set(debug_candidates /Od /Zi /RTC1 /JMC)
    set(release_candidates /O2 /Zi /guard:cf /Qspectre /sdl /GS)
    set(release_link_candidates /DEBUG /OPT:REF /OPT:ICF)
    set(probe_debug_context "/WX")
    set(probe_release_context "/WX")
else()
    set(debug_candidates
        -O0
        -ggdb3
        -fdebug-macro
        -fno-builtin
        -fno-inline
        -fno-omit-frame-pointer
        -fno-optimize-sibling-calls
        -fasynchronous-unwind-tables
        -fno-stack-protector
        -fno-eliminate-unused-debug-types
        -fkeep-inline-functions
        -fkeep-static-functions
        -grecord-gcc-switches
    )
    set(release_candidates
        -O2
        -g
        -fstack-protector-strong
        -fstack-clash-protection
        -fno-omit-frame-pointer
        -Wformat
        -Werror=format-security
    )
    if(TARGET_ARCH STREQUAL "x86_64")
        list(APPEND release_candidates -fcf-protection=full)
    elseif(TARGET_ARCH STREQUAL "aarch64")
        list(APPEND release_candidates -mbranch-protection=standard)
    endif()
    set(release_link_candidates
        -Wl,-z,relro
        -Wl,-z,now
        -Wl,-z,noexecstack
        -Wl,--as-needed
    )
    set(probe_debug_context "-Werror -g")
    set(probe_release_context "-Werror -O2 -Wformat")
endif()

set(CMAKE_REQUIRED_FLAGS "${probe_debug_context}")
keep_supported_flags(DEBUG_C_FLAGS debug ${debug_candidates})

set(CMAKE_REQUIRED_FLAGS "${probe_release_context}")
keep_supported_flags(RELEASE_C_FLAGS release ${release_candidates})
if(NOT MSVC)
    check_c_compiler_flag("-D_FORTIFY_SOURCE=3" ACCEPTS_FORTIFY_3)
    if(ACCEPTS_FORTIFY_3)
        list(APPEND RELEASE_C_FLAGS -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=3)
    else()
        check_c_compiler_flag("-D_FORTIFY_SOURCE=2" ACCEPTS_FORTIFY_2)
        if(ACCEPTS_FORTIFY_2)
            list(APPEND RELEASE_C_FLAGS -U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=2)
        endif()
    endif()
endif()
unset(CMAKE_REQUIRED_FLAGS)

keep_supported_link_flags(RELEASE_LINK_FLAGS release ${release_link_candidates})

check_pie_supported(LANGUAGES C)
if(CMAKE_C_LINK_PIE_SUPPORTED)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

string(JOIN " " CMAKE_C_FLAGS_DEBUG ${DEBUG_C_FLAGS})
string(JOIN " " CMAKE_C_FLAGS_RELEASE ${RELEASE_C_FLAGS})
if(NOT MSVC)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -DNDEBUG")
endif()
string(JOIN " " CMAKE_EXE_LINKER_FLAGS_RELEASE ${RELEASE_LINK_FLAGS})

if(ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_ok OUTPUT ipo_why LANGUAGES C)
    if(ipo_ok)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    else()
        message(WARNING "LTO requested but unavailable: ${ipo_why}")
    endif()
endif()

if(ENABLE_GPROF)
    add_compile_options(-pg)
    add_link_options(-pg)
endif()

if(SANITIZE)
    add_compile_options(-fsanitize=${SANITIZE} -fno-omit-frame-pointer)
    add_link_options(-fsanitize=${SANITIZE})
endif()

message(STATUS ">> Debug flags:   ${CMAKE_C_FLAGS_DEBUG}")
message(STATUS ">> Release flags: ${CMAKE_C_FLAGS_RELEASE}")
message(STATUS ">> Release link:  ${CMAKE_EXE_LINKER_FLAGS_RELEASE}")

# 4. Source Table
set(STUDY_SOURCES "")

function(study_source rel_path)
    cmake_parse_arguments(ARG "" "DIALECT" "WHEN" ${ARGN})

    if(ARG_DIALECT AND NOT ARG_DIALECT IN_LIST C_DIALECTS)
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
study_source(glibc/heap_fragmentation.c WHEN HAVE_GLIBC_LINUX)
study_source(gcc/statement_expression.c DIALECT gnu)
study_source(gcc/likely.c DIALECT gnu WHEN HAVE_CLOCK_GETTIME)

# 5. Target Generation
foreach(rel_path IN LISTS STUDY_SOURCES)
    get_filename_component(dir_name "${rel_path}" DIRECTORY)
    get_filename_component(filename "${rel_path}" NAME_WE)
    string(REPLACE "/" "_" unique_target "${dir_name}_${filename}")
    set(out_dir "${CMAKE_SOURCE_DIR}/bin/${BUILD_TAG}/${dir_name}")

    add_executable(${unique_target} "${CMAKE_SOURCE_DIR}/${rel_path}")
    set_target_properties(${unique_target} PROPERTIES
        OUTPUT_NAME "${filename}"
        FOLDER "${dir_name}"
        RUNTIME_OUTPUT_DIRECTORY "${out_dir}"
    )

    if(MSVC)
        set_target_properties(${unique_target} PROPERTIES PDB_OUTPUT_DIRECTORY "${out_dir}")
    endif()

    message(STATUS "      + Target created: ${unique_target} -> bin/${BUILD_TAG_SHOWN}/${dir_name}/${filename}")
endforeach()
