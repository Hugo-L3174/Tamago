# Tamago

This is a tamagotchi project (hence name) to try to build from scratch, based on the raspberry pi pico w

It depends on the [pico sdk](https://github.com/raspberrypi/pico-sdk/) and [pico-extras](https://github.com/raspberrypi/pico-extras).
Since version 2.0.0 of the pico sdk it also depends on having [picotool](https://github.com/raspberrypi/picotool) installed.
Here we assume they were cloned at the same level as this repository.

# Requirements
 1. Install CMake (at least version 3.13), and GCC cross compiler
   ```sh
   sudo apt install cmake gcc-arm-none-eabi libnewlib-arm-none-eabi libstdc++-arm-none-eabi-newlib
   ```
 2. Usual build instructions
   ```sh
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Release -DPICO_BOARD=pico_w
   make
   ```
 3. `tamago.elf` can be loaded via a debugger, or `tamago.uf2` can be dragged and dropped holding bootsel button when plugging the pico

# Images to display

  1. To get the source files to display they must first be converted to a 4 bits color scale (16 colors) bmp (using gimp: Image > Mode > Indexed)

  2. Then convert the images to header files using for example [bmp2src](https://github.com/cbm80amiga/bmp2src/) and convert using
   ```sh
    bmp2src image.bmp > image.h
   ```