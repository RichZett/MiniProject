# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(basic_01_blinkDemo)

target_sources(app PRIVATE
  src/main.c
  src/button_task.c
  src/led_task.c
  src/temp_task.c
  src/rtdb.c
  src/heater_task.c

)

