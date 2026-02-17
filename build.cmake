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
execute_process(
    COMMAND ${CMAKE_COMMAND} -S ${ROOT_DIR} -B ${BUILD_DIR}
    RESULT_VARIABLE result
)

if(NOT result EQUAL 0)
    message(FATAL_ERROR "Configuration failed!")
endif()

message(STATUS "\n[Step 2] Building...")
set(BUILD_CMD ${CMAKE_COMMAND} --build ${BUILD_DIR})

# Force Debug configuration on Multi-Config generators (like Visual Studio)
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
