cmake_minimum_required(VERSION 3.10)
project(c-programming C)

if(MSVC)
    add_compile_options(/utf-8 /W4)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -g -pedantic)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

function(create_targets_from_dir dir_name)
    if(NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${dir_name})
        return()
    endif()

    # Automatically create the output directory under bin/
    file(MAKE_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${dir_name}")

    file(GLOB sources "${dir_name}/*.c")

    foreach(src ${sources})
        get_filename_component(target_name ${src} NAME_WE)
        set(unique_target "${dir_name}_${target_name}")
        add_executable(${unique_target} ${src})
        set_target_properties(${unique_target} PROPERTIES 
            RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin/${dir_name}"
        )
    endforeach()
endfunction()

create_targets_from_dir("math")
create_targets_from_dir("pointer")
create_targets_from_dir("std")
create_targets_from_dir("type")

if(MSVC)
    create_targets_from_dir("msvc")
else()
    create_targets_from_dir("gcc")
endif()
