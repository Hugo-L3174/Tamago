#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"

typedef struct{
    bool gameOver_;
    const int width_;
    const int height_;
    int x_, z_, score_;
    bool jumping_;
    // vertical velocity of dino
    float velocityZ_;
    const float gravity_;
    // jumping power by default
    float jumpStrength_;
} dinoGame;


typedef struct{
    int posX_;
    int height_;
} cactus;
