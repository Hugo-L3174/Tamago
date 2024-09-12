#include "dino.h"


void Setup() 
{
    gameOver_ = false;
    x_ = 5;
    z_ = height_ - 2;
    score_ = 0;
    jumping_ = false;
    velocityZ_ = 0;
    // init rand seed 
    srand(time(0));
}

void Draw() 
{
    system("cls"); 
    
    // Upper screen
    for (int i = 0; i < width_; i++) cout << "#";
    cout << endl;

    // Draw game
    for (int i = 0; i < height_; i++) 
    {
        for (int j = 0; j < width_; j++) 
        {
            bool printed = false;

            // Draw dino
            if (i == y && j == x) 
            {
                cout << "D";
                printed = true;
            }

            // Draw cacti
            for (const auto& cactus : cacti_) 
            {
                if (j == cactus.posX_ && i >= height_ - 2 - cactus.height_ && i < height_ - 1) 
                {
                    cout << "|";
                    printed = true;
                    break;
                }
            }

            if (!printed) 
            {
                if (i == height_ - 1) cout << "#";  // ground
                else cout << " ";
            }
        }
        cout << endl;
    }

    // print score
    cout << "Score: " << score << endl;
}

void Input() 
{
    // Do this as a callback
    // While pressing and on the ground, add jumping power
    if (_kbhit()) 
    {
        char key = _getch();
        if (key == ' ' && y == height_ - 2) 
        {  
            // If on the ground, jump
            jumping_ = true;
            velocityY_ = jumpStrength_; 
        }
    }
}

void Logic() {
    // Apply gravity if in the air
    if (jumping_ || z_ < height_ - 2) 
    {
        velocityZ_ += gravity_; 
        z_ += velocityZ_;

        // Max height
        if (z_ < 0) 
        {
            z_ = 0;
            velocityZ_ = 0;
        }

        // Touching ground
        if (z_ >= height_ - 2) 
        {
            z_ = height_ - 2;
            velocityZ_ = 0;
            jumping_ = false;
        }
    }

    // Move cacti
    for (auto& cactus : cacti_) 
    {
        cactus.posX--;
    }

    // Remove offscreen cacti
    if (!cacti_.empty() && cacti_[0].posX_ < 0) 
    {
        cacti_.erase(cacti_.begin());
        score_++;
    }

    // Create new cacti at random intervals (1/20 iteration)
    if (rand() % 20 == 0) 
    {
        Cactus newCactus;
        newCactus.posX_ = width_ - 2;
        // random height
        newCactus.height_ = rand() % 3 + 1; 
        cacti_.push_back(newCactus);
    }

    // Check collision
    for (const auto& cactus : cacti_) 
    {
        if (cactus.posX_ == x_ && z_ >= height_ - 2 - cactus.height_) 
        {
            gameOver_ = true;
        }
    }
}

int main() 
{
    Setup();
    while (!gameOver_) 
    {
        Draw();
        Input();
        Logic();
        Sleep(100);
    }

    cout << "Game Over!" << endl;
    return 0;
}