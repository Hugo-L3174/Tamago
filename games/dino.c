#include "dino.h"
#include "GUI_Paint.h"


dinoGame dinoSetup(uint64_t * randSeed) 
{
    dinoGame Game = {false, 128, 98, 49, 0, 0, 0, false, 0, -0.3, 0, NULL};
    midButtonPressedDino_ = false;
    rButtonPressedDino_ = false;
    // init rand seed 
    srand(*randSeed);
    return Game;
}

void dinoDraw(dinoGame *game, uint8_t **screenBuffer, const unsigned char **spriteFramePtr) 
{
    // Character
    Paint_Clear(BLACK);
    Paint_DrawImage_Flipped(spriteFramePtr[leftmouthopenFrame], game->charaPosX_, 100 - game->charaHeight_ - game->charaPosZ_, 38, 49);
    // Paint_DrawImage(spriteFramePtr[leftmouthopenFrame], 30, 30, 38, 49);
    // Paint_DrawImage(spriteFramePtr[goingleftFrame], game->x_, game->z_, 38, 49);

    // Ground
    Paint_DrawRectangle(0, 101, 128, 105, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Cacti
    cactus * currentCact = game->firstCactus_;
    while (currentCact != NULL)
    {
        for (size_t i = 0; i < currentCact->height_; i++)
        {
            Paint_DrawRectangle(currentCact->posX_, 94 - 6*i, currentCact->posX_ + 6, 100 - 6*i, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
        }
        
        currentCact = currentCact->next_;
    }

    // Score
    Paint_DrawString_EN(0, 110, "Score:", &Font12, 0x8, 0x1);
    Paint_DrawNum(44, 110, game->score_, &Font12, 0, 0x8, 0x1);
}



void dinoLogic(dinoGame *game) 
{
    // If not during a jump
    if (!game->jumping_)
    {
        // If on the ground and M button pressed, adding jump strength
        if (midButtonPressedDino_)
        {
            game->jumpStrength_ += 1;
        }
        // If there is strength and button is not pressed, then it was released and we jump
        else if (!(game->jumpStrength_ == 0) && !midButtonPressedDino_)
        {
            game->velocityZ_ = game->jumpStrength_;
            game->jumpStrength_ = 0;
            game->jumping_ = true;
        }
        
    }
    
    // If we are jumping apply gravity and update z pos
    if (game->jumping_) 
    {
        game->charaPosZ_ += game->velocityZ_;
        game->velocityZ_ += game->gravity_; 

        // Touching ground
        if (game->charaPosZ_ <= 0) 
        {
            game->charaPosZ_ = 0;
            game->velocityZ_ = 0;
            game->jumping_ = false;
        }

        // Max height
        if (game->charaPosZ_ >= game->height_ - game->charaHeight_) 
        {
            game->charaPosZ_ = game->height_ - game->charaHeight_;
            // Stop going up
            if (game->velocityZ_ > 0)
            {
                game->velocityZ_ = 0;
            }
        }
    }


    // Move cacti
    cactus * currentCact = game->firstCactus_;
    while (currentCact != NULL)
    {
        currentCact->posX_ -= 2;
        currentCact = currentCact->next_;
    }

    // Remove offscreen cacti
    // Only need to check first element since they move step by step
    if (game->firstCactus_ != NULL)
    {
        if (game->firstCactus_->posX_ < 0)
        {
            // Setting new first cactus in list
            currentCact = game->firstCactus_;
            game->firstCactus_ = game->firstCactus_->next_;
            // freeing offscreen cactus
            free(currentCact);
        }
    }
    

    // Create new cacti at random intervals (~1/200 iteration)
    currentCact = game->firstCactus_;
    if (rand() % 200 == 0)
    {
        if (currentCact == NULL)
        {
            // there are no cacti, ie current is the last, allocate
            game->firstCactus_ = (cactus *) malloc(sizeof(cactus));
            game->firstCactus_->posX_ = game->width_ - 2;
            // random height 1-4
            game->firstCactus_->height_ = rand() % 3 + 1;
            game->firstCactus_->next_ = NULL;
        }
        else
        {
            // there is at least one cactus, get to the end of the list then allocate
            while (currentCact->next_ != NULL)
            {
                currentCact = currentCact->next_;
            }
            // current is now the last
            currentCact->next_ = (cactus *) malloc(sizeof(cactus));
            currentCact = currentCact->next_;

            currentCact->posX_ = game->width_ - 2;
            // random height 1-4
            currentCact->height_ = rand() % 3 + 1;
            currentCact->next_ = NULL;
        }
        
    }

    // Check collision
    currentCact = game->firstCactus_;
    while (currentCact !=NULL)
    {
        // logic for width check:
        // sprite is 38 wide total but body only 20 wide -> left is posX + 8 and right is posX + 28
        // cactus is 6 wide -> left is posX and right is posX + 6
        // intersection if right of character overlaps with left cactus or left of character with right cactus
        // height check is simply height * 6 (cactus "unit" height)
        if (game->charaPosX_ + 28 >= currentCact->posX_ && game->charaPosX_ + 8 <= currentCact->posX_ + 6 && game->charaPosZ_ <= currentCact->height_ * 6) 
        {
            game->gameOver_ = true;
        }
        currentCact = currentCact->next_;
    }
    game->score_++;
    
}

int playDino(uint8_t *screenBuffer, const unsigned char **spriteFramePtr, uint64_t * randSeed, void (*dispFunction)(uint8_t *screenBuffer), int (*debouceCheck)(), void (*waitFunction)(uint32_t waitTime)) 
{
    dinoGame game = dinoSetup(randSeed);

    // make sure buttons start debounced
    rButtonPressedDino_ = false;
    midButtonPressedDino_ = false;

    while (!(game.gameOver_ || rButtonPressedDino_)) 
    {
        dinoLogic(&game);
        dinoDraw(&game, &screenBuffer, spriteFramePtr);
        dispFunction(screenBuffer);
    }

    Paint_Clear(BLACK);
    Paint_DrawString_EN(25, 30, "Game over !", &Font12, 0x3, 0xe);
    Paint_DrawString_EN(30, 70, "Score:", &Font12, 0x8, 0x1);
    Paint_DrawNum(74, 70, game.score_, &Font12, 0, 0x8, 0x1);
    dispFunction(screenBuffer);
    waitFunction(4000);
    return game.score_;
}

// Callback correction: find a way to keep the same callback across everything: button presses must update a global flag that should be used by all logics

