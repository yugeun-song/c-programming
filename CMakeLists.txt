cmake_minimum_required(VERSION 3.10)
project(c-programming C)

# Compiler-specific flags
if(MSVC)
    add_compile_options(/utf-8 /W4)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -g -pedantic)
endif()

# Global output directory defaults (overridden per target below)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

# Function to generate executables mirroring source structure
function(create_targets_from_dir dir_name)
    if(NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${dir_name})
        return()
    endif()

    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${dir_name}")
    file(GLOB sources "${dir_name}/*.c")

    foreach(src ${sources})
        # Extract pure filename (e.g., 'test' from 'math/test.c')
        get_filename_component(filename ${src} NAME_WE)
        
        # Internal Target Name: Must be unique globally (e.g., 'math_test')
        set(unique_target "${dir_name}_${filename}")

        add_executable(${unique_target} ${src})

        # Properties to achieve 1:1 mapping:
        # 1. OUTPUT_NAME: Forces the binary to be 'test.exe', not 'math_test.exe'
        # 2. RUNTIME_OUTPUT_DIRECTORY: Places it in 'bin/math/'
        set_target_properties(${unique_target} PROPERTIES 
            OUTPUT_NAME "${filename}"
            FOLDER "${dir_name}"
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
        )
        
        if(MSVC)
            set_target_properties(${unique_target} PROPERTIES 
                PDB_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
                PDB_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
            )
        endif()
    endforeach()
endfunction()

# Register module directories
create_targets_from_dir("math")
create_targets_from_dir("pointer")
create_targets_from_dir("std")
create_targets_from_dir("type")

if(MSVC)
    create_targets_from_dir("msvc")
else()
    create_targets_from_dir("gcc")
endif()
