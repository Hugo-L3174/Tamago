#include "C4Nar/c1.h"
#include "C4Nar/farfetchd.h"
#include "pokemon.h"
#include "C4Nar/farfetchd_gen3.h"
#include "monky2.h"
#include "chips.h"
#include "C4Nar/pascal.h"

#include "Mame/frontawkward.h"
#include "Mame/frontdown.h"
#include "Mame/fronthappy.h"
#include "Mame/frontmeh.h"
#include "Mame/frontwaving.h"
#include "Mame/goingleft.h"
#include "Mame/leftmouthop.h"
#include "Mame/sittinghappy.h"
#include "Mame/sittingmouthop.h"
#include "Mame/sittingsurprised.h"

// Sprite structure
// We want to make it an array of pointers (1 pointer per frame, which is an array of unsigned char)
typedef const unsigned char *spriteFramePtr;


spriteFramePtr Duck[] = {can128rgb1, can128rgb2, can128rgb3, can128rgb4, can128rgb5, can128rgb6, 
                        can128rgb7, can128rgb8, can128rgb9, can128rgb10, can128rgb11, can128rgb13, 
                        can128rgb14, can128rgb15, can128rgb16, can128rgb17, can128rgb18};

// Mametchi sprites are all 38x49 pixels
spriteFramePtr Mametchi[] = {fronthappy, frontwaving, frontmeh, frontawkward, 
                        goingleft, leftmouthop, frontdown, sittinghappy, 
                        sittingmouthop, sittingsurprised};

// possible species
enum species {mametchi}; 
// names (for info)
const char *tamaSpecies[] = {"Mametchi"};

// array of pointers to all possible sprites
spriteFramePtr *tamaSprites[] = {Mametchi};