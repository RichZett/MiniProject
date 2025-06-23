# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.21.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(basic_01_blinkDemo)

# ON / OFF switch for unit testing, switch off when building full project 
set(UNIT_TESTING ON)

target_sources(app PRIVATE
  src/button_task.c
  src/led_task.c
  src/temp_task.c
  src/rtdb.c
  src/heater_task.c

)

# ----------- UNIT TEST BUILD (non-Zephyr) --------------------
if(UNIT_TESTING)
    message(STATUS "Building Unity unit tests") 

    # Add directories needed for Unit testing
    target_include_directories(app PRIVATE
        src
        src/test
        src/Unity
    )

    # Add specific codes needed
    target_sources(app PRIVATE
        src/tests/test_temp_task.c
        src/Unity/unity.c
    )

else()
    message(STATUS "Building the entire project")
    target_sources(app PRIVATE src/main.c)
endif()
