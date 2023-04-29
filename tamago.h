#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "pico/time.h"


#include "lib/Config/DEV_Config.h"
#include "lib/OLED/OLED_1in5.h"
#include "lib/GUI/GUI_Paint.h"
// #include "lib/GUI/GUI_BMPfile.h" // not useful on microcontroller
#include "lib/Fonts/fonts.h"
// #include "Debug.h"


#include "pic/C4Nar/c1.h"
/*#include "pic/C4Nar/c2.h"
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
#include "pic/C4Nar/c18.h"*/

#include <stdlib.h>
#include <math.h>

int OLED_1in5_test(void);
int OLED_pic(void);

int OLED_canarticho(void);

int tama_init(void);
int enter_name(void);

int debug_print(void);

int screen_init(void); // if I want to use this I need the image object to be global



enum species {canar, evangelion, tamago, calcifer, navet, howl, laputarob, dqslime}; 
int species_nb = 8;

struct kreatur
{
    char name[20];
    int hunger; // Sould go up as tama becomes hungry
    int sick;   // Same
    int age;
    int happiness;
    int iq;
    enum species type;

};

struct kreatur tama = {"zero", 0, 0, 0, 10, 10, 0};

