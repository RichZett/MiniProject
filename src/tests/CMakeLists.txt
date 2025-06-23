# tests/CMakeLists.txt

cmake_minimum_required(VERSION 3.20)
project(UnityTests C)

# Add Unity as a subdirectory
add_subdirectory(../Unity Unity_build)

# Include Unity headers and source files
include_directories(../Unity/src)
include_directories(../src)

# Define the test executable
add_executable(test_runner
    test_temp_task.c
    ../src/temp_task.c
    ../src/rtdb.c
)

target_link_libraries(test_runner unity)
