#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"


#include "lib/Config/DEV_Config.h"
#include "lib/OLED/OLED_1in5.h"
#include "lib/GUI/GUI_Paint.h"
// #include "lib/GUI/GUI_BMPfile.h" // not useful on microcontroller
#include "lib/Fonts/fonts.h"
// #include "Debug.h"


#include "pic/C4Nar/c1.h"
#include "pic/C4Nar/c2.h"
#include "pic/C4Nar/c3.h"
#include "pic/C4Nar/c4.h"
#include "pic/C4Nar/c5.h"
#include "pic/C4Nar/c6.h"
#include "pic/C4Nar/c7.h"
#include "pic/C4Nar/c8.h"
#include "pic/C4Nar/c9.h"
#include "pic/C4Nar/c10.h"
#include "pic/C4Nar/c11.h"
#include "pic/C4Nar/c12.h"
#include "pic/C4Nar/c13.h"
#include "pic/C4Nar/c14.h"
#include "pic/C4Nar/c15.h"
#include "pic/C4Nar/c16.h"
#include "pic/C4Nar/c17.h"
#include "pic/C4Nar/c18.h"

#include <stdlib.h>
#include <math.h>

int OLED_1in5_test(void);
int OLED_pic(void);

int OLED_canarticho(void);