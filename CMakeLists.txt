cmake_minimum_required(VERSION 3.22)

project(BlinkProject C)

if(NOT DEFINED ENV{LOCAL_GCC_PATH})
    message(FATAL_ERROR "Please set the LOCAL_GCC_PATH in paths.local")
endif()

if(NOT DEFINED ENV{PROJECT_SRC_DIR})
    message(FATAL_ERROR "Please set the PROJECT_SRC_DIR in paths.local")
endif()

set(PROJECT_SRC_DIR $ENV{PROJECT_SRC_DIR})

set(CMAKE_C_COMPILER $ENV{LOCAL_GCC_PATH}/bin/arm-none-eabi-gcc)

set(CMAKE_C_FLAGS "-mcpu=cortex-m0 -mthumb -nostdlib")

set(LINKER_FILE ${PROJECT_SRC_DIR}/src/linker.ld)

set(CMAKE_EXE_LINKER_FLAGS "-T ${LINKER_FILE}")

set(SOURCE_DIR ${PROJECT_SRC_DIR}/src)

add_executable(blink.elf ${SOURCE_DIR}/main.c ${SOURCE_DIR}/startup.c)

#target_link_libraries(blink.elf ${CMAKE_EXE_LINKER_FLAGS})

