cmake_minimum_required(VERSION 3.12)
project(c-programming C)

# 1. Compiler Options
if(MSVC)
    add_compile_options(/utf-8 /W4)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    message(STATUS ">> Platform: Windows (MSVC)")
else()
    add_compile_options(-Wall -Wextra -g -pedantic)
    message(STATUS ">> Platform: Linux/Unix (GCC/Clang)")
endif()

# 2. Global Output Directory Settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

# 3. Function to create targets from directory
function(create_targets_from_dir dir_name)
    set(target_dir "${CMAKE_SOURCE_DIR}/${dir_name}")

    if(NOT IS_DIRECTORY ${target_dir})
        message(STATUS "   [Skip] Directory not found: ${dir_name}")
        return()
    endif()

    # Create output directory explicitly
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${dir_name}")
    
    # Scan source files (CONFIGURE_DEPENDS enables auto-detection)
    file(GLOB sources CONFIGURE_DEPENDS "${dir_name}/*.c")

    list(LENGTH sources source_count)
    if(source_count EQUAL 0)
        message(STATUS "   [Skip] No .c files in: ${dir_name}")
        return()
    endif()

    message(STATUS ">> Scanning ${dir_name}...")

    foreach(src ${sources})
        get_filename_component(filename ${src} NAME_WE)
        set(unique_target "${dir_name}_${filename}")

        if(NOT TARGET ${unique_target})
            add_executable(${unique_target} ${src})
            message(STATUS "      + Target created: ${filename} (bin/${dir_name}/${filename})")

            # Configure output paths for binaries
            set_target_properties(${unique_target} PROPERTIES 
                OUTPUT_NAME "${filename}"
                FOLDER "${dir_name}"
                RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
                RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
                RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
            )
            
            # Configure PDB paths for MSVC debugging
            if(MSVC)
                set_target_properties(${unique_target} PROPERTIES 
                    PDB_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
                    PDB_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
                )
            endif()
        endif()
    endforeach()
endfunction()

# 4. Module Registration
# Universal Modules
create_targets_from_dir("math")
create_targets_from_dir("pointer")
create_targets_from_dir("std")
create_targets_from_dir("type")

# Platform-Specific Modules
if(MSVC)
    create_targets_from_dir("msvc")
else()
    create_targets_from_dir("gcc")
endif()