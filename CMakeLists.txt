cmake_minimum_required(VERSION 3.12)
project(c-programming C)

# 1. Compiler Options
if(MSVC)
    add_compile_options(/utf-8 /W4)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    message(STATUS ">> Platform: Windows (MSVC)")
else()
    add_compile_options(-Wall -Wextra -pedantic -ggdb3 -O0 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fasynchronous-unwind-tables)
    message(STATUS ">> Platform: Linux/Unix (GCC/Clang)")
endif()

# 2. Auto-Discovery & Target Generation
message(STATUS ">> Scanning for .c files in subdirectories...")

# Scan source files (CONFIGURE_DEPENDS enables auto-detection)
file(GLOB_RECURSE ALL_C_FILES CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/*.c")

foreach(src IN LISTS ALL_C_FILES)
    file(RELATIVE_PATH rel_path "${CMAKE_SOURCE_DIR}" "${src}")

    # Skip build, bin, and hidden directories
    if(rel_path MATCHES "^(build|bin|\\.git)/")
        continue()
    endif()

    get_filename_component(dir_name "${rel_path}" DIRECTORY)
    get_filename_component(filename "${src}" NAME_WE)

    # Skip files in the root directory
    if("${dir_name}" STREQUAL "")
        continue()
    endif()

    string(REPLACE "/" "_" unique_target "${dir_name}_${filename}")

    if(NOT TARGET ${unique_target})
        add_executable(${unique_target} "${src}")
        message(STATUS "      + Target created: ${unique_target} -> bin/${dir_name}/${filename}")

        set(out_dir "${CMAKE_SOURCE_DIR}/bin/${dir_name}")

        # Configure output paths for binaries
        set_target_properties(${unique_target} PROPERTIES
            OUTPUT_NAME "${filename}"
            FOLDER "${dir_name}"
            RUNTIME_OUTPUT_DIRECTORY "${out_dir}"
            RUNTIME_OUTPUT_DIRECTORY_DEBUG "${out_dir}"
            RUNTIME_OUTPUT_DIRECTORY_RELEASE "${out_dir}"
        )

        # Configure PDB paths for MSVC debugging
        if(MSVC)
            set_target_properties(${unique_target} PROPERTIES
                PDB_OUTPUT_DIRECTORY "${out_dir}"
                PDB_OUTPUT_DIRECTORY_DEBUG "${out_dir}"
            )
        endif()
    endif()
endforeach()