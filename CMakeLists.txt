cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
include(pico-sdk/pico_sdk_init.cmake)

project(tamago)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_library(Oled STATIC 
    ${CMAKE_CURRENT_LIST_DIR}/lib/Config/DEV_Config.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/OLED/OLED_1in5.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/GUI/GUI_Paint.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/GUI/GUI_BMPfile.c 
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font8.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font12.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font16.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font20.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font24.c
)

target_include_directories(Oled PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/Config/
    ${CMAKE_CURRENT_LIST_DIR}/lib/OLED/
    ${CMAKE_CURRENT_LIST_DIR}/lib/GUI/
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/
)

target_link_libraries(Oled pico_stdlib hardware_spi)


add_executable(tamago
    tamago.c
)

target_include_directories(tamago PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/Config/
    ${CMAKE_CURRENT_LIST_DIR}/lib/OLED/
    ${CMAKE_CURRENT_LIST_DIR}/lib/GUI/
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(tamago pico_stdlib hardware_spi Oled)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(tamago)