# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initalize pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "C:/Users/Anton/Pico/pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(oximeter_pico_sw C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(oximeter_pico_sw oximeter_pico_sw.c 7seg.c digital_filter.c heartbeat_detector.c)

pico_set_program_name(oximeter_pico_sw "oximeter_pico_sw")
pico_set_program_version(oximeter_pico_sw "0.1")

pico_enable_stdio_uart(oximeter_pico_sw 0)
pico_enable_stdio_usb(oximeter_pico_sw 1)

# Add the standard library to the build
target_link_libraries(oximeter_pico_sw pico_stdlib)
pico_generate_pio_header(oximeter_pico_sw ${CMAKE_CURRENT_LIST_DIR}/7seg.pio)

# Add any user requested libraries
target_link_libraries(oximeter_pico_sw
        hardware_pio
        hardware_clocks
        hardware_pll
        hardware_dma
        hardware_irq
        hardware_adc
)

pico_add_extra_outputs(oximeter_pico_sw)

