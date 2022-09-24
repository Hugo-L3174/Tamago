cmake_minimum_required(VERSION 3.13)

# initialize pico-sdk from submodule
include(pico-sdk/pico_sdk_init.cmake)

project(Tamago)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(tamago
    tamago.c
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(tamago pico_stdlib)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(tamago)