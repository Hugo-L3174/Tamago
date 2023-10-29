#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "pico/time.h"
#include "pico/multicore.h"
//#include "hardware/rtc.h"


#include "lib/Config/DEV_Config.h"
#include "lib/OLED/OLED_1in5.h"
#include "lib/GUI/GUI_Paint.h"
#include "lib/Fonts/fonts.h"
// #include "Debug.h"

#include "lib/Buzzer/Buzz.h"

#include "lib/UPS/Pico_UPS.h"

#include "pic/C4Nar/c1.h"
#include "pic/C4Nar/farfetchd.h"
#include "pic/pokemon.h"
#include "pic/C4Nar/farfetchd_gen3.h"
#include "pic/monky.h"
#include "pic/monky2.h"

#include <stdlib.h>
#include <math.h>

#define SCREEN
#define BUTTONS

// 1x1x4 4 by for
const unsigned char test[4] = 
{ 0xf0,0x00,0x00,0x00/*,0x23,0x78,0xaa,0xaa,
  0x21,0x00,0x00,0x00,0x21,0x22,0xa7,0xaa,
  0xaa,0x78,0x77,0xaa,0xaa,0x25,0x00,0x00,
  0x21,0x00,0x00,0x00,0x21,0x22,0xa7,0xaa*/
};

int OLED_1in5_test(void);

int OLED_canarticho(void);

int buzzTest(void);

int hardware_setup(void); 

// Initialize pet variables
int tama_init(void);
int enter_name(void);

int debug_buttons(void);

int debug_battery(void);

int debug_images(void);

int debug_overlay(void);

// Initialize mem allocation for image buffer and reset it to black
int image_init(void);

// game logic loop to run on second core (core 1)
int game_loop(void);

int timers_test(void);



enum species {canar, evangelion, tamago, calcifer, navet, howl, laputarob, dqslime}; 
int species_nb = 8;

typedef struct{
    int xOrig;
    int yOrig;
    bool goingRight;
} sprite;

typedef struct{
    char name[20];
    volatile int hunger; // Sould go up as tama becomes hungry
    int sick;   // Same
    int age;
    int happiness;
    int iq;
    volatile sprite sprite;
    enum species type;

} kreatur;

kreatur tama_ = {"zero", 0, 0, 0, 10, 10, {0, 40, true}, 0};

const unsigned char *c4nar[18] = {can128rgb1, can128rgb2, can128rgb3, can128rgb4, can128rgb5, can128rgb6, can128rgb7, can128rgb8, can128rgb9,
                                    can128rgb10, can128rgb11, can128rgb13, can128rgb14, can128rgb15, can128rgb16, can128rgb17, can128rgb18};

UBYTE *ScreenImage;

//previously ((OLED_1in5_WIDTH%2==0)? (OLED_1in5_WIDTH/2): (OLED_1in5_WIDTH/2+1)) * OLED_1in5_HEIGHT
// width divided by 2 because 1 address addresses 2 pixels on x axis
UWORD ScreenSize = (OLED_1in5_WIDTH/2) * OLED_1in5_HEIGHT ;

enum menu {zero, infos, c, d, e, f, g, h, i};

enum menu cursor = zero;

// Update flags
volatile bool spriteToUpdate_ = false;

struct repeating_timer timerHunger, spriteMove;

bool hunger_callback(struct repeating_timer *t) {
    tama_.hunger +=1;
    return true;
}

bool spriteMove_callback(struct repeating_timer *t) {
    if (tama_.sprite.goingRight && tama_.sprite.xOrig <= 49)
    {
        tama_.sprite.xOrig += 15;
        // how to change y?
    }
    else if (tama_.sprite.goingRight && tama_.sprite.xOrig > 49)
    {
        tama_.sprite.goingRight = false;
        tama_.sprite.xOrig -= 15;
    }
    else if (!tama_.sprite.goingRight && tama_.sprite.xOrig >= 15)
    {
        tama_.sprite.xOrig -= 15;
    }
    else if (!tama_.sprite.goingRight && tama_.sprite.xOrig < 15)
    {
        tama_.sprite.goingRight = true;
        tama_.sprite.xOrig += 15;
    }
    else // error case: going up
    {
        tama_.sprite.yOrig += 10;
    }
    
    spriteToUpdate_ = true;
    
    return true;
}

// algo logic: core 0 initializes everything and core 1 should update the looping idle animation 
// (internal shared variable between the cores?)
// core 1 : timeout then update internal image to display
// image will be constructed by core 0 then displayed each loop
// Question: how to wait for image modification before displaying? probably setting a flag that is reset after each update
