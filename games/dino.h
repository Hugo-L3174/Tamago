#include "tamago.h"

// TODO maybe put it in a game struct to create when running the game
// otherwise all global variables
bool gameOver_;
const int width_ = 50;
const int height_ = 20;
int x_, z_, score_;
bool jumping_;
// vertical velocity of dino
float velocityZ_;
const float gravity_ = -0.5;
// jumping power by default
float jumpStrength_ = 4.0;

struct Cactus {
    int posX_;
    int height_;
};

vector<Cactus> cacti_;