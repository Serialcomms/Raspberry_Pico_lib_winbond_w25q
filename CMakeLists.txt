# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.2.0)
set(toolchainVersion 14_2_Rel1)
set(picotoolVersion 2.2.0-a4)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
set(PICO_BOARD pimoroni_pico_plus2_rp2350 CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(lib_winbond_w25q_demo C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_subdirectory(lib_winbond_w25q)

# Add executable. Default name is the project name, version 0.1

add_executable(lib_winbond_w25q_demo main.c )

pico_set_program_name(lib_winbond_w25q_demo "lib_winbond_w25q_demo")
pico_set_program_version(lib_winbond_w25q_demo "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(lib_winbond_w25q_demo 1)
pico_enable_stdio_usb(lib_winbond_w25q_demo 0)

# Add the standard library to the build
target_link_libraries(lib_winbond_w25q_demo
        pico_stdlib)

# Add the standard include files to the build
target_include_directories(lib_winbond_w25q_demo PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(lib_winbond_w25q_demo 
        hardware_spi
        hardware_dma
        pico_bootrom
        pico_status_led
        lib_winbond_w25q
)

pico_add_extra_outputs(lib_winbond_w25q_demo)