#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#ifndef BUTTONS
#define BUTTONS
#define LBUTT           12 
#define MBUTT           11
#define RBUTT           10
#endif

#ifndef FRAMENAMES
#define FRAMENAMES
// possible frames for sprites
enum frames {fronthappyFrame, 
             frontwavingFrame, 
             frontmehFrame, 
             frontawkwardFrame, 
             goingleftFrame, 
             leftmouthopenFrame, 
             goingfrontFrame, 
             sittinghappyFrame, 
             sittingmouthopenFrame, 
             sittingsurprisedFrame
            };
#endif

typedef struct cactus{
    int posX_;
    int height_;
    struct cactus * next_;
} cactus;


typedef struct{
    bool gameOver_;
    const int width_;
    const int height_;
    const int charaHeight_;
    int charaPosX_, charaPosZ_, score_;
    bool jumping_;
    // vertical velocity of dino
    float velocityZ_;
    const float gravity_;
    // jumping power
    float jumpStrength_;
    cactus * firstCactus_;
} dinoGame;


dinoGame dinoSetup(uint64_t * randSeed);
void dinoDraw(dinoGame *game, uint8_t **screenBuffer, const unsigned char **spriteFramePtr);
void dinoLogic(dinoGame *game);
// buttons logic to be called as GPIO irq callback
// void dinoInputCallback(unsigned int gpio, uint32_t events);
int playDino(uint8_t *screenBuffer, const unsigned char **spriteFramePtr, uint64_t * randSeed, void (*dispFunction)(uint8_t *screenBuffer), int (*debouceCheck)(), void (*waitFunction)(uint32_t waitTime));

volatile bool midButtonPressedDino_;
volatile bool rButtonPressedDino_;