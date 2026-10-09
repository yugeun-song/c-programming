include(CheckLinkerFlag)
include(CheckPIESupported)
include(CMakeCheckCompilerFlagCommonPatterns)
check_compiler_flag_common_patterns(flag_fail_patterns)

set(gnu_strict -Werror)
set(gnu_common -Wall -Wextra -pedantic)
set(gnu_debug
    -O0
    -ggdb3
    -fdebug-macro
    -fno-builtin
    -fno-inline
    -fno-omit-frame-pointer
    -mno-omit-leaf-frame-pointer
    -fno-optimize-sibling-calls
    -fasynchronous-unwind-tables
    -fno-stack-protector
    -fno-stack-clash-protection
    -fcf-protection=none
    -mbranch-protection=none
    -U_FORTIFY_SOURCE
    -ffp-contract=off
    -mno-relax
    -fno-eliminate-unused-debug-types
    -fkeep-inline-functions
    -fkeep-static-functions
    -fkeep-persistent-storage-variables
    -grecord-gcc-switches
)
set(gnu_debug_link "")
set(gnu_release
    -O2
    -g
    -fstack-protector-strong
    -fstack-clash-protection
    -fno-omit-frame-pointer
    -mno-omit-leaf-frame-pointer
    -Wformat
    -Werror=format-security
    -fcf-protection=full
    -mbranch-protection=standard
    -U_FORTIFY_SOURCE
    -D_FORTIFY_SOURCE=3
)
set(gnu_release_link
    -Wl,-z,relro
    -Wl,-z,now
    -Wl,-z,noexecstack
    -Wl,--as-needed
)
set(gnu_optimized -O2)

set(msvc_strict /WX)
set(msvc_common /utf-8 /W4 /D_CRT_SECURE_NO_WARNINGS)
set(msvc_debug /Od /Zi /RTC1 /JMC)
set(msvc_debug_link /DEBUG /INCREMENTAL)
set(msvc_release /O2 /Zi /guard:cf /Qspectre /sdl /GS)
set(msvc_release_link /DEBUG /OPT:REF /OPT:ICF)
set(msvc_optimized /O2)

option(ENABLE_LTO "Link-time optimization in Release" OFF)
option(ENABLE_GPROF "Instrument every config for gprof" OFF)
set(SANITIZE "" CACHE STRING "Sanitizers for every config, e.g. address,undefined")

function(probe_flags out config)
    cmake_parse_arguments(PARSE_ARGV 2 ARG "LINK" "" "CONTEXT;FLAGS")
    # try_compile forwards CMAKE_C_FLAGS_<config>; probe in that config with the defaults cleared
    string(TOUPPER ${config} config_upper)
    set(CMAKE_TRY_COMPILE_CONFIGURATION ${config})
    set(CMAKE_C_FLAGS_${config_upper} "")
    set(accepted "")
    foreach(flag IN LISTS ARG_FLAGS)
        list(JOIN ARG_CONTEXT " " context)
        list(JOIN accepted " " prefix)
        string(MD5 context_id "${config} ${context} ${prefix}")
        string(SUBSTRING ${context_id} 0 8 context_id)
        if(ARG_LINK)
            string(MAKE_C_IDENTIFIER "LINKS_${flag}_${context_id}" probe)
            check_linker_flag(C "${flag}" ${probe})
        else()
            string(MAKE_C_IDENTIFIER "ACCEPTS_${flag}_${context_id}" probe)
            set(CMAKE_REQUIRED_FLAGS "${${FLAG_SYNTAX}_strict} ${context} ${prefix} ${flag}")
            check_c_source_compiles("int main(void) { return 0; }" ${probe} ${flag_fail_patterns})
        endif()
        if(${probe})
            list(APPEND accepted "${flag}")
        endif()
    endforeach()
    set(${out} "${accepted}" PARENT_SCOPE)
endfunction()

function(report label)
    list(JOIN ARGN " " text)
    message(STATUS ">> ${label}: ${text}")
endfunction()

set(requested "")
if(ENABLE_GPROF)
    list(APPEND requested -pg)
endif()
if(SANITIZE)
    list(APPEND requested -fsanitize=${SANITIZE})
endif()
set(granted ${requested})
set(common ${${FLAG_SYNTAX}_common})
set(OPTIMIZED_FLAGS ${${FLAG_SYNTAX}_optimized})
foreach(config IN LISTS BUILD_CONFIGS)
    string(TOLOWER ${config} table)
    string(TOUPPER ${config} config_upper)
    if(NOT DEFINED ${FLAG_SYNTAX}_${table} OR NOT DEFINED ${FLAG_SYNTAX}_${table}_link)
        message(FATAL_ERROR "No ${FLAG_SYNTAX} flag table for ${config}.")
    endif()
    probe_flags(common ${config} FLAGS ${common})
    probe_flags(compile ${config} CONTEXT ${common} FLAGS ${${FLAG_SYNTAX}_${table}})
    probe_flags(link ${config} LINK FLAGS ${${FLAG_SYNTAX}_${table}_link})
    probe_flags(OPTIMIZED_FLAGS ${config} CONTEXT ${common} ${compile} FLAGS ${OPTIMIZED_FLAGS})
    probe_flags(granted ${config} CONTEXT ${common} ${compile} FLAGS ${granted})
    list(JOIN compile " " CMAKE_C_FLAGS_${config_upper})
    list(JOIN link " " CMAKE_EXE_LINKER_FLAGS_${config_upper})
    set(dropped ${${FLAG_SYNTAX}_${table}} ${${FLAG_SYNTAX}_${table}_link})
    foreach(flag IN LISTS compile link)
        list(REMOVE_ITEM dropped "${flag}")
    endforeach()
    report("${config}" ${compile})
    report("${config} link" ${link})
    if(dropped)
        report("${config} dropped" ${dropped})
    endif()
endforeach()
add_compile_options(${common})
add_compile_definitions($<$<CONFIG:Release>:NDEBUG>)
report("Every config" ${common} ${granted})
if(NOT granted STREQUAL requested)
    message(FATAL_ERROR "${CMAKE_C_COMPILER_ID} rejects part of: ${requested}")
endif()
if(granted)
    # CMAKE_C_FLAGS also reaches the link step of gcc and clang, which -pg and -fsanitize need
    list(JOIN granted " " granted)
    string(APPEND CMAKE_C_FLAGS " ${granted}")
endif()

check_pie_supported(LANGUAGES C)
if(CMAKE_C_LINK_PIE_SUPPORTED)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE OFF)
if(ENABLE_LTO)
    if(NOT DEFINED CACHE{LTO_SUPPORTED})
        include(CheckIPOSupported)
        check_ipo_supported(RESULT supported LANGUAGES C)
        set(LTO_SUPPORTED ${supported} CACHE INTERNAL "")
    endif()
    if(LTO_SUPPORTED)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    else()
        message(WARNING "Release builds without LTO: ${CMAKE_C_COMPILER_ID} fails check_ipo_supported.")
    endif()
endif()
report("Release LTO" ${CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE})