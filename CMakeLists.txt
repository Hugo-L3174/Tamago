cmake_minimum_required(VERSION 3.13)

set(PICO_BOARD pico_w CACHE STRING "Board type")

# # initialize pico-sdk from submodule
# include(pico-sdk/pico_sdk_init.cmake)
# initialize pico-sdk (assuming it is at the same level as the Tamago repo)
include(../pico-sdk/pico_sdk_init.cmake)

# pico extras should be added for the deep sleep functions
include(../pico-extras/external/pico_extras_import.cmake)

project(tamago)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_library(GUI STATIC 
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font8.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font12.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font16.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font20.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/font24.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/GUI/GUI_Paint.c
)

target_include_directories(GUI PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/Fonts/
    ${CMAKE_CURRENT_LIST_DIR}/lib/GUI/
)

add_library(TamaBoard STATIC 
    ${CMAKE_CURRENT_LIST_DIR}/lib/Config/DEV_Config.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/OLED/OLED_1in5.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/Buzzer/Buzz.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/UPS/Pico_UPS.c
    ${CMAKE_CURRENT_LIST_DIR}/lib/UPS/power_status.c
)

target_include_directories(TamaBoard PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/Config/
    ${CMAKE_CURRENT_LIST_DIR}/lib/OLED/
    ${CMAKE_CURRENT_LIST_DIR}/lib/Buzzer/
    ${CMAKE_CURRENT_LIST_DIR}/lib/UPS/
)
                                                                        
target_link_libraries(TamaBoard
    pico_stdlib 
    pico_cyw43_arch_none # for battery reading (this gpio is controlled by cyw43) and bluetooth
    hardware_spi 
    hardware_pwm 
    hardware_i2c # i2c optional: was for UPS
    hardware_adc 
    hardware_sleep
    hardware_clocks
)

add_library(Bluetooth STATIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetooth/client.c
)

target_link_libraries(Bluetooth
    pico_stdlib
    pico_btstack_ble # for bluetooth
    pico_btstack_cyw43 # for bluetooth
    pico_cyw43_arch_none # for battery reading (VSYS gpio is controlled by cyw43 on pico_w) and bluetooth
    TamaBoard 
)

target_include_directories(Bluetooth PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/lib/Bluetooth/ # For btstack config
)
target_compile_definitions(Bluetooth PUBLIC
    RUNNING_AS_CLIENT=1 # This sets the compile flag in btstack config
)

# generate gatt header for bluetooth
# pico_btstack_make_gatt_header(Bluetooth PRIVATE "${CMAKE_CURRENT_LIST_DIR}/lib/Bluetooth/temp_sensor.gatt")

# run bluetooth cmake in subdirectory
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/lib/Bluetooth/)

add_library(Games STATIC
    ${CMAKE_CURRENT_LIST_DIR}/games/dino.c
)

target_include_directories(Games PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/games/
)

target_link_libraries(Games
    GUI
)

add_executable(tamago
    tamago.c 
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(tamago 
    TamaBoard
    Bluetooth
    GUI
    Games
)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(tamago)