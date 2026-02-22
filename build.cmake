# OS-agnostic build script running on CMake itself
cmake_minimum_required(VERSION 3.10)

get_filename_component(ROOT_DIR ${CMAKE_CURRENT_LIST_FILE} DIRECTORY)
set(BUILD_DIR "${ROOT_DIR}/build")

message(STATUS "-------------------------------------------------")
message(STATUS "Build Script Started")
message(STATUS "Root:  ${ROOT_DIR}")
message(STATUS "Build: ${BUILD_DIR}")
message(STATUS "-------------------------------------------------")

message(STATUS "[Step 1] Configuring...")

# Clean previous build cache to prevent generator conflicts (especially between Ninja/MSVC)
if(EXISTS "${BUILD_DIR}/CMakeCache.txt")
    message(STATUS ">> Cleaning stale build cache...")
    file(REMOVE_RECURSE "${BUILD_DIR}")
endif()

# Determine the generator based on OS
if(WIN32)
    # Force Visual Studio to prevent CMake from accidentally picking MinGW/Ninja
    message(STATUS ">> Windows detected: Probing for Visual Studio (2026 and newer)...")
    
    # VS 2026 (v18), VS 2022 (v17), etc.
    set(vs_years 2026 2022 2019)
    set(vs_majors 18 17 16)
    
    set(found_generator FALSE)
    list(LENGTH vs_years len)
    math(EXPR range "${len} - 1")

    foreach(i RANGE ${range})
        list(GET vs_years ${i} year)
        list(GET vs_majors ${i} major)
        set(current_gen "Visual Studio ${major} ${year}")

        execute_process(
            COMMAND ${CMAKE_COMMAND} -G "${current_gen}" -A x64 -S ${ROOT_DIR} -B ${BUILD_DIR}
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
        message(FATAL_ERROR "MSVC not found! Please ensure Visual Studio 2026 or later is installed.")
    endif()
else()
    # On Linux/Unix: Use Unix Makefiles
    message(STATUS ">> Linux/Unix detected: Using Unix Makefiles...")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -G "Unix Makefiles" -S ${ROOT_DIR} -B ${BUILD_DIR}
        RESULT_VARIABLE result
    )
endif()

if(NOT result EQUAL 0)
    message(FATAL_ERROR "Configuration failed!")
endif()

message(STATUS "\n[Step 2] Building...")
set(BUILD_CMD ${CMAKE_COMMAND} --build ${BUILD_DIR})

# Force Debug configuration on Multi-Config generators
if(WIN32)
    list(APPEND BUILD_CMD --config Debug)
endif()

execute_process(
    COMMAND ${BUILD_CMD}
    RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "Build failed!")
endif()

message(STATUS "\n[Success] Binaries are in 'bin/' directory.")