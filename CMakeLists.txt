cmake_minimum_required(VERSION 3.13)
project(c-programming C)

# 1. Compiler Options
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

# 2. Source Lists & Target Generation
set(UNIVERSAL_SOURCES
    memory/struct_padding.c
    std/hello_world.c
    type/format_string.c
)

set(GCC_SOURCES
    gcc/likely.c
)

set(MSVC_SOURCES
)

set(PROJECT_SOURCES ${UNIVERSAL_SOURCES})
if(MSVC)
    list(APPEND PROJECT_SOURCES ${MSVC_SOURCES})
else()
    list(APPEND PROJECT_SOURCES ${GCC_SOURCES})
endif()

foreach(rel_path IN LISTS PROJECT_SOURCES)
    get_filename_component(dir_name "${rel_path}" DIRECTORY)
    get_filename_component(filename "${rel_path}" NAME_WE)
    string(REPLACE "/" "_" unique_target "${dir_name}_${filename}")
    set(out_dir "${CMAKE_SOURCE_DIR}/bin/${dir_name}")

    if(MSVC)
        add_executable(${unique_target} "${CMAKE_SOURCE_DIR}/${rel_path}")
        message(STATUS "      + Target created: ${unique_target} -> bin/${dir_name}/${filename}")

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
        message(STATUS "      + Target created: ${unique_target} -> bin/${dir_name}/${filename} (trace)")

        add_executable(${unique_target}_opt "${CMAKE_SOURCE_DIR}/${rel_path}")
        target_compile_options(${unique_target}_opt PRIVATE ${OPT_C_FLAGS})
        set_target_properties(${unique_target}_opt PROPERTIES
            OUTPUT_NAME "${filename}_opt"
            FOLDER "${dir_name}"
            RUNTIME_OUTPUT_DIRECTORY "${out_dir}"
        )
        message(STATUS "      + Target created: ${unique_target}_opt -> bin/${dir_name}/${filename}_opt (optimized)")
    endif()
endforeach()

# 3. Assembly Listing Generation
if(NOT MSVC)
    set(ASM_OPT_LEVELS O0 O1 O2 O3 Os Ofast Og)
    set(ASM_ARCHS x86_64 aarch64 riscv64)
    set(asm_compiler_x86_64 x86_64-linux-gnu-gcc)
    set(asm_compiler_aarch64 aarch64-linux-gnu-gcc)
    set(asm_compiler_riscv64 riscv64-linux-gnu-gcc)

    set(available_archs "")
    foreach(arch IN LISTS ASM_ARCHS)
        if(CMAKE_SYSTEM_PROCESSOR STREQUAL "${arch}")
            set(asm_cc_${arch} "${CMAKE_C_COMPILER}")
            list(APPEND available_archs "${arch}")
        else()
            find_program(ASM_CC_${arch} "${asm_compiler_${arch}}")
            if(ASM_CC_${arch})
                set(asm_cc_${arch} "${ASM_CC_${arch}}")
                list(APPEND available_archs "${arch}")
            else()
                message(STATUS ">> [asm] ${arch}: compiler not found (${asm_compiler_${arch}}) - skipping")
            endif()
        endif()
    endforeach()

    if(available_archs)
        message(STATUS ">> [asm] Architectures: ${available_archs} / Levels: ${ASM_OPT_LEVELS}")
        file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/asm_stamps")

        set(asm_stamps "")
        foreach(rel_path IN LISTS PROJECT_SOURCES)
            get_filename_component(dir_name "${rel_path}" DIRECTORY)
            get_filename_component(filename "${rel_path}" NAME_WE)
            string(REPLACE "/" "_" flat_name "${dir_name}_${filename}")

            foreach(arch IN LISTS available_archs)
                set(asm_out_dir "${CMAKE_SOURCE_DIR}/asm/${dir_name}/${filename}/gcc/${arch}")
                file(MAKE_DIRECTORY "${asm_out_dir}")

                foreach(level IN LISTS ASM_OPT_LEVELS)
                    set(stamp "${CMAKE_BINARY_DIR}/asm_stamps/${flat_name}_gcc_${arch}_${level}.stamp")
                    add_custom_command(
                        OUTPUT "${stamp}"
                        COMMAND ${CMAKE_COMMAND}
                            -DASM_COMPILER=${asm_cc_${arch}}
                            -DASM_LEVEL=${level}
                            -DASM_SOURCE=${CMAKE_SOURCE_DIR}/${rel_path}
                            -DASM_OUTPUT=${asm_out_dir}/${level}.s
                            -DASM_LOG=${asm_out_dir}/${level}.log
                            -P ${CMAKE_SOURCE_DIR}/cmake/asm_gen.cmake
                        COMMAND ${CMAKE_COMMAND} -E touch "${stamp}"
                        DEPENDS "${CMAKE_SOURCE_DIR}/${rel_path}" "${CMAKE_SOURCE_DIR}/cmake/asm_gen.cmake"
                        BYPRODUCTS "${asm_out_dir}/${level}.s"
                        COMMENT "[asm] ${rel_path} -> asm/${dir_name}/${filename}/gcc/${arch}/${level}.s"
                        VERBATIM
                    )
                    list(APPEND asm_stamps "${stamp}")
                endforeach()
            endforeach()
        endforeach()

        add_custom_target(asm ALL DEPENDS ${asm_stamps})
    else()
        message(STATUS ">> [asm] No architectures available - skipping assembly generation")
    endif()
endif()
