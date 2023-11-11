#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "pico/time.h"
#include "pico/multicore.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
//#include "hardware/rtc.h"


#include "lib/Config/DEV_Config.h"
#include "lib/OLED/OLED_1in5.h"
#include "lib/GUI/GUI_Paint.h"
#include "lib/Fonts/fonts.h"
#include "lib/Buzzer/Buzz.h"
#include "lib/UPS/Pico_UPS.h"
#include "lib/Battery/power_status.h"


#include "pic/C4Nar/c1.h"
#include "pic/C4Nar/farfetchd.h"
#include "pic/pokemon.h"
#include "pic/C4Nar/farfetchd_gen3.h"
#include "pic/monky2.h"
#include "pic/chips.h"
#include "pic/C4Nar/pascal.h"

#include "utils/menus.h"


// defines to select which hardware to initialize
#define SCREEN          1
#define UPS             0
#define BUTTONS         1
#define BATTERY_MONITOR 0
#define BUZZER          0


// total number of possibilities for species (for random selection)
#define species_nb  7


/****************************************************************
*      Internal types structs and definitions
*
****************************************************************/

// possible species
enum species {canar, evangelion, tamago, calcifer, navet, laputarob, dqslime}; 
const char *tamaSpecies[8] = {"Canar", "Angel", "Tamago", "Calcifer", "Navet", "Roboto", "Gluant"};

// menus options for cursor
enum mainMenu {none, food, play, wash, heal, comm, bedtime, infos, settings};
enum foodMenu {junk, drink, foodCancel};
enum playMenu {hop, playCancel};
enum washMenu {shower, washCancel};
enum healMenu {pill, healCancel};
enum commMenu {search, commCancel};
enum bedtimeMenu {light, bedtimeCancel};
enum infosMenu {name, age, hunger, happiness, species, infosCancel};
enum settingsMenu {brightness, settingsCancel};

// strings to display to select menu options
const char *foodOptions[3] = {"Junk food", "Boire", "Retour"};
const char *playOptions[2] = {"Sauter", "Retour"};
const char *washOptions[2] = {"Douche", "Retour"};
const char *healOptions[2] = {"Pilule", "Retour"};
const char *commOptions[2] = {"Recherche", "Retour"};
const char *bedtimeOptions[2] = {"Eteindre", "Retour"};
const char *infoOptions[6] = {"Nom:", "Age:", "Faim:", "Bonheur:", "Espece:", "Retour"};
const char *settingsOptions[2] = {"Luminosite", "Retour"};

// possible screens
enum screens {mainScreen, foodScreen, playScreen, washScreen, healScreen, commScreen, bedtimeScreen, infosScreen, settingsScreen};

// sprite position structure
typedef struct{
    int xOrig;
    int yOrig;
    bool goingRight;
} sprite;

// creature structure
typedef struct{
    char name[20];
    volatile int hunger; // Sould go up as tama becomes hungry
    int sick;   // Same
    int age;
    int happiness;
    int iq;
    volatile sprite sprite;
    char species[20];

} kreatur;

// game internal vars
typedef struct{
    volatile enum mainMenu mainCursor;
    volatile enum foodMenu foodCursor;
    volatile enum playMenu playCursor;
    volatile enum washMenu washCursor;
    volatile enum healMenu healCursor;
    volatile enum commMenu commCursor;
    volatile enum bedtimeMenu bedtimeCursor;
    volatile enum infosMenu infoCursor;
    volatile enum settingsMenu settingsCursor;
    volatile enum screens currentScreen;
} game;




/****************************************************************
*                   Functions definitions
*
****************************************************************/

int OLED_1in5_test(void);

int OLED_canarticho(void);

int buzzTest(void);

int hardware_setup(void); 

// Initialize pet variables
int tama_init(void);

int enter_name(void);

int debug_buttons(void);

int debug_UPS(void);

int debug_battery(void)
{
    
};

int debug_images(void);

int debug_overlay(void);

// Initialize mem allocation for image buffer and reset it to black
int image_init(void);

// menu logic to be called as GPIO irq callback
void menu_logic(uint gpio, uint32_t events);

int timers_test(void);

// Refresh main menu icons area
void RefreshIcons(void);

// Refresh sprite in center area
void RefreshSprite(void);

// Refresh whole screen for menu change
void RefreshMenu(void);

/****************************************************************
*                   Internal global variables
*
****************************************************************/

// screen memory pointer
UBYTE *ScreenImage_;

// creature instance
kreatur tama_;

// game instance
game game_ = {none, junk, mainScreen};

// Update flags
volatile bool spriteToUpdate_ = false;
volatile bool iconsToUpdate_ = false;
volatile bool menuToUpdate_ = false;
volatile bool cursorToUpdate_ = false;

// Timers for timed callbacks
struct repeating_timer hungerTimer_, spriteMoveTimer_;

// timer for debounce control
unsigned long time;
const int delayTime = 200; // Delay between every push button

// Battery monitoring variables
bool old_battery_status;
float old_voltage;
bool battery_status = true;
char *power_str = "UNKNOWN";

// Sprite structure
const unsigned char *duck[18] = {can128rgb1, can128rgb2, can128rgb3, can128rgb4, can128rgb5, can128rgb6, 
                                 can128rgb7, can128rgb8, can128rgb9, can128rgb10, can128rgb11, can128rgb13, 
                                 can128rgb14, can128rgb15, can128rgb16, can128rgb17, can128rgb18};


/****************************************************************
*                   Callback functions
*
****************************************************************/

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
