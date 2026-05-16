#ifndef GAMES_H
#define GAMES_H

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef BUTTONS
#  define BUTTONS
#  define LBUTT 12
#  define MBUTT 11
#  define RBUTT 10
#endif

#ifndef FRAMENAMES
#  define FRAMENAMES
// possible frames for sprites
enum frames
{
  fronthappyFrame,
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

typedef struct
{
  volatile bool * lButton;
  volatile bool * mButton;
  volatile bool * rButton;
} gameInput;

#endif
