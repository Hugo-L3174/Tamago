cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
include(pico-sdk/pico_sdk_init.cmake)

project(tamago)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_library(Fonts STATIC 
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font8.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font12.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font16.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font20.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font24.c
)

target_include_directories(Fonts PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/
)

add_library(TamaBoard STATIC 
    ${CMAKE_CURRENT_LIST_DIR}/lib/Config/DEV_Config.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/OLED/OLED_1in5.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/GUI/GUI_Paint.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Buzzer/Buzz.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/UPS/Pico_UPS.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Battery/power_status.c
)

target_include_directories(TamaBoard PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/Config/
    ${CMAKE_CURRENT_LIST_DIR}/lib/OLED/
    ${CMAKE_CURRENT_LIST_DIR}/lib/GUI/
    ${CMAKE_CURRENT_LIST_DIR}/lib/Buzzer/
    ${CMAKE_CURRENT_LIST_DIR}/lib/UPS/
    ${CMAKE_CURRENT_LIST_DIR}/lib/Battery/
)

target_link_libraries(TamaBoard Fonts pico_stdlib hardware_spi hardware_pwm hardware_i2c hardware_adc pico_cyw43_arch_none pico_multicore)


add_executable(tamago
    tamago.c 
)

# target_include_directories(tamago PUBLIC
#     ${CMAKE_CURRENT_LIST_DIR}/lib/Config/
#     ${CMAKE_CURRENT_LIST_DIR}/lib/OLED/
#     ${CMAKE_CURRENT_LIST_DIR}/lib/GUI/
#     # ${CMAKE_CURRENT_LIST_DIR}/lib/Buzzer/
# )

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(tamago pico_stdlib hardware_spi hardware_pwm TamaBoard)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(tamago)