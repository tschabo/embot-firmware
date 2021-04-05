cmake_minimum_required(VERSION 3.12)

include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(stitcher C CXX ASM )

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(stitcher main.cpp
    Parser.h
    Parser.cpp
    CommandBuffer.h
    SimpleStepper.h
    SimpleStepper.cpp
    Mover.h
    Mover.cpp)

target_link_libraries(stitcher pico_stdlib)

pico_enable_stdio_usb(stitcher 1)
pico_enable_stdio_uart(stitcher 0)
pico_add_extra_outputs(stitcher)