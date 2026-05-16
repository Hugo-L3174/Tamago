#include "games.h"

typedef struct cactus
{
  int posX_;
  int height_;
  struct cactus * next_;
} cactus;

typedef struct
{
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
void dinoDraw(dinoGame * game, uint8_t ** screenBuffer, const unsigned char ** spriteFramePtr);
void dinoLogic(dinoGame * game, volatile bool * buttonPressed);
int playDino(uint8_t * screenBuffer,
             const unsigned char ** spriteFramePtr,
             uint64_t * randSeed,
             void (*dispFunction)(uint8_t * screenBuffer),
             void (*waitFunction)(uint32_t waitTime),
             gameInput * input);
