#include "dino.h"
#include "GUI_Paint.h"

dinoGame dinoSetup() 
{
    dinoGame Game = {false, 128, 98, 49, 0, 0, 0, false, 0, -1, 0, NULL};
    midButtonPressedDino_ = false;
    rButtonPressedDino_ = false;
    // init rand seed 
    srand(0);
    return Game;
}

void dinoDraw(dinoGame *game, uint8_t **screenBuffer, const unsigned char **spriteFramePtr) 
{
    // Character
    Paint_DrawImage_Flipped(spriteFramePtr[leftmouthopenFrame], game->x_, 100 - game->charaHeight_ - game->z_, 38, 49);
    // Paint_DrawImage(spriteFramePtr[leftmouthopenFrame], 30, 30, 38, 49);
    // Paint_DrawImage(spriteFramePtr[goingleftFrame], game->x_, game->z_, 38, 49);

    // Ground
    Paint_DrawRectangle(0, 101, 128, 105, WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);

    // Cacti
    cactus * currentCact = game->firstCactus_;
    while (currentCact != NULL)
    {
        int posX = currentCact->posX_;
        int height = currentCact->height_;
        for (size_t i = 0; i < height; i++)
        {
            Paint_DrawRectangle(posX, 31 + 6*i, posX + 6, 37 + 6*i, WHITE, DOT_PIXEL_1X1, DRAW_FILL_EMPTY);
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
        game->z_ += game->velocityZ_;
        game->velocityZ_ += game->gravity_; 

        // Touching ground
        if (game->z_ <= 0) 
        {
            game->z_ = 0;
            game->velocityZ_ = 0;
        }

        // Max height
        if (game->z_ >= game->height_ - game->charaHeight_) 
        {
            game->z_ = game->height_ - game->charaHeight_;
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
        currentCact->posX_--;
        currentCact = currentCact->next_;
    }

    // Remove offscreen cacti
    currentCact = game->firstCactus_;
    while (currentCact !=NULL)
    {
        if (currentCact->posX_ < 0)
        {
            // Setting new first cactus in list
            game->firstCactus_ = game->firstCactus_->next_;
            // freeing offscreen cactus
            free(currentCact);
        }
        
        currentCact = currentCact->next_;
    }

    // Create new cacti at random intervals (1/20 iteration)
    currentCact = game->firstCactus_;
    if (rand() % 20 == 0) 
    {
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

    // Check collision
    currentCact = game->firstCactus_;
    while (currentCact !=NULL)
    {
        if (game->x_ == currentCact->posX_ && game->z_ <= currentCact->height_) 
        {
            game->gameOver_ = true;
        }
        currentCact = currentCact->next_;
    }
    game->score_++;
    
}

int playDino(uint8_t *screenBuffer, const unsigned char **spriteFramePtr, void (*dispFunction)(uint8_t *screenBuffer), int (*debouceCheck)()) 
{
    dinoGame game = dinoSetup();

    while (!game.gameOver_) 
    {
        dinoLogic(&game);
        dinoDraw(&game, &screenBuffer, spriteFramePtr);
        dispFunction(screenBuffer);
        // Sleep(50);
    }

    Paint_DrawString_EN(110, 5, "Game over !", &Font12, 0x3, 0xe);
    dispFunction(screenBuffer);
    return game.score_;
}

// 1. pass screen buffer + update screen flag
// 2. run logic with game as arg
// -> change buttons irq to jump etc
// 3. run update buffer with buffer and flag as arg + set flag to true at the end
// 4. outside of this find a way to keep polling update flag to update screen (callback on flag?)

