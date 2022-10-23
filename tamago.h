#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"


#include "lib/Config/DEV_Config.h"
#include "lib/OLED/OLED_1in5.h"
#include "lib/GUI/GUI_Paint.h"
#include "lib/GUI/GUI_BMPfile.h"
#include "lib/Fonts/fonts.h"
// #include "Debug.h"

#include <stdlib.h> // malloc() free()
#include <math.h>

int OLED_1in5_test(void);