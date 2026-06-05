cmake_minimum_required(VERSION 3.12)

execute_process(
    COMMAND "${ASM_COMPILER}" -S "-${ASM_LEVEL}" -o "${ASM_OUTPUT}" "${ASM_SOURCE}"
    RESULT_VARIABLE result
    ERROR_VARIABLE error_output
)

if(result EQUAL 0)
    if(EXISTS "${ASM_LOG}")
        file(REMOVE "${ASM_LOG}")
    endif()
else()
    if(EXISTS "${ASM_OUTPUT}")
        file(REMOVE "${ASM_OUTPUT}")
    endif()
    file(WRITE "${ASM_LOG}" "Assembly generation failed.\nCompiler: ${ASM_COMPILER}\nLevel: -${ASM_LEVEL}\nSource: ${ASM_SOURCE}\n\n${error_output}")
    message(STATUS "[asm] generation failed: ${ASM_SOURCE} (-${ASM_LEVEL}) - see ${ASM_LOG}")
endif()
