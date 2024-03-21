#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "pico/time.h"
// #include "pico/multicore.h"
#include "hardware/adc.h"
#include "pico/cyw43_arch.h"
//#include "hardware/rtc.h"

#include "lib/Config/DEV_Config.h"
#include "lib/OLED/OLED_1in5.h"
#include "lib/GUI/GUI_Paint.h"
#include "lib/Fonts/fonts.h"
#include "lib/Buzzer/Buzz.h"
#include "lib/UPS/Pico_UPS.h"
#include "lib/UPS/power_status.h"

#include "lib/Bluetooth/client.h"

#include "pic/sprites.h"


// total number of possibilities for species (for random selection)
#define species_nb  2


/****************************************************************
*      Internal types structs and definitions
*
****************************************************************/

// menus options for cursor
enum mainMenu {none, food, play, wash, heal, comm, bedtime, infos, settings};
enum foodMenu {junk, drink, foodCancel};
enum playMenu {hop, playCancel};
enum washMenu {shower, washCancel};
enum healMenu {pill, healCancel};
enum commMenu {broadcast, search, commCancel};
enum bedtimeMenu {light, bedtimeCancel};
enum infosMenu {name, age, hunger, happiness, species, infosCancel};
enum settingsMenu {brightness, settingsCancel};

// strings to display to select menu options
const char *foodOptions[3] = {"Junk food", "Boire", "Retour"};
const char *playOptions[2] = {"Sauter", "Retour"};
const char *washOptions[2] = {"Douche", "Retour"};
const char *healOptions[2] = {"Pilule", "Retour"};
const char *commOptions[3] = {"Emettre", "Recherche", "Retour"};
const char *bedtimeOptions[2] = {"Eteindre", "Retour"};
const char *infoOptions[6] = {"Nom:", "Age:", "Faim:", "Bonheur:", "Espece:", "Retour"};
const char *settingsOptions[2] = {"Luminosite", "Retour"};

// possible screens
enum screens {mainScreen, foodScreen, playScreen, washScreen, healScreen, commScreen, bedtimeScreen, infosScreen, settingsScreen};

// possible frames for sprites
enum frames {fronthappyFrame, frontwavingFrame, frontmehFrame, frontawkwardFrame, goingleftFrame, leftmouthopenFrame, goingfrontFrame, sittinghappyFrame, sittingmouthopenFrame, sittingsurprisedFrame};

// sprite structure
typedef struct{
    int xOrig;
    int yOrig;
    bool goingRight;
    spriteFramePtr *frames;
    spriteFramePtr currentFrame;
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


// basic structure for movement: holds previous and objective coordinates
typedef struct{
    int prevX;
    int prevY;
    int targetX;
    int targetY;
} move;


/****************************************************************
*                   Functions definitions
*
****************************************************************/

int OLED_1in5_test(void);

int buzzTest(void);

// init all peripherals (pins, functions, spi communication)
int hardware_setup(void); 

// Initialize pet variables
int tama_init(void);

int enter_name(void);

int debug_UPS(void);

int debug_battery(void);

int debug_images(void);

int debug_dino(void);

int debug_overlay(void);

int debug_bt(void);

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

// game mechanics

// Passes array of sprite frames for the food
// should be blocking function
void feed(spriteFramePtr *foodSprite, int foodVal);
void walk(int movX, int movY);
void idleWalk();

// stops alarms and irqs before powering down
void prePowerDown();

// reinitializes alarms etc to get game running again after powering back up
void postPowerUp();

void BluetoothComm(int mode);

// polling of battery level
void batteryPoll();

// GUI utils
void DrawAllIcons(void);
void ClearIconsArea(UWORD Color);
void UserEnterString(void);
void UserInitRTC();

/****************************************************************
*                   Internal global variables
*
****************************************************************/

// screen memory pointer
UBYTE *ScreenImage_;

// creature instance
kreatur tama_;

// game instance
game game_ = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Update flags
volatile bool spriteToUpdate_ = false;
volatile bool iconsToUpdate_ = false;
volatile bool menuToUpdate_ = false;
volatile bool cursorToUpdate_ = false;
volatile int bluetoothMode_ = 0; 
volatile bool batteryToUpdate_ = true;
bool displayToUpdate_ = false;

// Timers for timed callbacks
struct repeating_timer hungerTimer_, spriteMoveTimer_, batteryPollTimer_;

// timer for debounce control
unsigned long time;
const int delayTime = 200; // Delay between every push button

// movement variable
move movement_;

// Battery monitoring variables
bool old_battery_status;
float old_voltage;
bool battery_status = true;
char power_src[] = "BatInconnue";
uint8_t batteryVal;
bool lowBattery = false;

/****************************************************************
*                   Callback functions
*
****************************************************************/

bool hunger_callback(struct repeating_timer *t) {
    tama_.hunger +=1;
    return true;
}

// to be called regularly to make the idle sprite animation
bool spriteMove_callback(struct repeating_timer *t) {
    idleWalk();
    return true;
}

bool pollBatt_callback(struct repeating_timer *t) {
    batteryToUpdate_ = true;
    return true;
}

// TODO maybe only 1 callback to walk, with user data number of frames to decomp movement

// called by the idle animation : 1st frame
int64_t walk_1_callback(alarm_id_t id, void *user_data) {
    // move* movement = (move*)user_data;
    tama_.sprite.currentFrame = tama_.sprite.frames[leftmouthopenFrame];
	tama_.sprite.xOrig = movement_.prevX + (movement_.targetX * 1.0/3.0);
	tama_.sprite.yOrig = movement_.prevY + (movement_.targetY * 1.0/3.0);
	spriteToUpdate_ = true;
    return 0;
}

// called by the idle animation : 2nd frame
int64_t walk_2_callback(alarm_id_t id, void *user_data) {
    tama_.sprite.currentFrame = tama_.sprite.frames[goingleftFrame];
    tama_.sprite.xOrig = movement_.prevX + (movement_.targetX * 2.0/3.0);
	tama_.sprite.yOrig = movement_.prevY + (movement_.targetY * 2.0/3.0);
	spriteToUpdate_ = true;
    return 0;
}

// called by the idle animation : 3rd frame
int64_t walk_3_callback(alarm_id_t id, void *user_data) {
    tama_.sprite.currentFrame = tama_.sprite.frames[leftmouthopenFrame];
    tama_.sprite.xOrig = movement_.prevX + movement_.targetX;
	tama_.sprite.yOrig = movement_.prevY + movement_.targetY;
	spriteToUpdate_ = true;
    return 0;
}

// called by the idle animation : final frame (stop moving)
int64_t walk_4_callback(alarm_id_t id, void *user_data) {
    tama_.sprite.currentFrame = tama_.sprite.frames[fronthappyFrame];
    spriteToUpdate_ = true;
    return 0;
}

// algo logic: core 0 initializes everything and core 1 should update the looping idle animation 
// (internal shared variable between the cores?)
// core 1 : timeout then update internal image to display
// image will be constructed by core 0 then displayed each loop
// Question: how to wait for image modification before displaying? probably setting a flag that is reset after each update
