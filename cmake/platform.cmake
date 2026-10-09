include(CheckCSourceCompiles)
include(CheckSymbolExists)
set(CMAKE_REQUIRED_QUIET ON)

if(NOT CMAKE_SYSTEM_NAME MATCHES "^(Linux|Windows)$")
    message(FATAL_ERROR "Unsupported target OS '${CMAKE_SYSTEM_NAME}'. This project targets Linux and Windows.")
endif()
string(TOLOWER "${CMAKE_SYSTEM_NAME}" TARGET_OS)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU")
    execute_process(
        COMMAND "${CMAKE_C_COMPILER}" -dumpfullversion
        OUTPUT_VARIABLE gcc_driver_version
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
endif()
if(CMAKE_C_COMPILER_ID STREQUAL "GNU" AND gcc_driver_version STREQUAL CMAKE_C_COMPILER_VERSION)
    set(TARGET_CC gcc)
    set(FLAG_SYNTAX gnu)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang" AND CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
    set(TARGET_CC clang)
    set(FLAG_SYNTAX gnu)
elseif(CMAKE_C_COMPILER_ID STREQUAL "Clang" AND CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    set(TARGET_CC clang)
    set(FLAG_SYNTAX msvc)
elseif(CMAKE_C_COMPILER_ID STREQUAL "MSVC")
    set(TARGET_CC msvc)
    set(FLAG_SYNTAX msvc)
else()
    message(FATAL_ERROR "${CMAKE_C_COMPILER} is not GCC, Clang or MSVC (CMake ID '${CMAKE_C_COMPILER_ID}' ${CMAKE_C_COMPILER_VERSION}).")
endif()
set(KNOWN_DIALECTS gcc clang msvc gnu)
set(C_DIALECTS ${TARGET_CC} ${FLAG_SYNTAX})
list(REMOVE_DUPLICATES C_DIALECTS)

set(arch_predicates
    x86_64 "defined(__x86_64__) || defined(_M_X64)"
    aarch64 "defined(__aarch64__) || defined(_M_ARM64)"
    riscv64 "defined(__riscv) && __riscv_xlen == 64"
)
set(TARGET_ARCH "")
while(arch_predicates)
    list(POP_FRONT arch_predicates arch predicate)
    check_c_source_compiles("#if !(${predicate})\n#error\n#endif\nint main(void) { return 0; }" ARCH_IS_${arch})
    if(ARCH_IS_${arch})
        list(APPEND TARGET_ARCH ${arch})
    endif()
endwhile()
list(LENGTH TARGET_ARCH arch_count)
if(NOT arch_count EQUAL 1)
    message(FATAL_ERROR "Unsupported target architecture. This project targets x86_64, aarch64 and riscv64.")
endif()
if(TARGET_OS STREQUAL "windows" AND NOT TARGET_ARCH STREQUAL "x86_64")
    message(FATAL_ERROR "Unsupported combination: windows on ${TARGET_ARCH}. Windows builds target x86_64 only.")
endif()

set(TARGET_TAG ${TARGET_ARCH}-${TARGET_OS}-${TARGET_CC})
if(EXPECTED_TARGET AND NOT EXPECTED_TARGET STREQUAL TARGET_TAG)
    message(FATAL_ERROR "Preset ${EXPECTED_TARGET} resolved to ${TARGET_TAG}.")
endif()

check_symbol_exists(__GLIBC__ "features.h" HAVE_GLIBC)
set(CMAKE_REQUIRED_DEFINITIONS -D_POSIX_C_SOURCE=199309L)
check_symbol_exists(clock_gettime "time.h" HAVE_CLOCK_GETTIME)
unset(CMAKE_REQUIRED_DEFINITIONS)

list(JOIN C_DIALECTS "," dialects_shown)
message(STATUS ">> Target: ${TARGET_TAG} | ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}, ${FLAG_SYNTAX} syntax | dialects: ${dialects_shown}")