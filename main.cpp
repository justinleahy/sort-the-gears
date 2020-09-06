#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "olcPGEX_AdditionalColors.h"
#include <iostream>
#include <random>
#include <string>
#include <vector>
class Gear {
private:
    // Location of the gear
    int x;
    int y;

    bool bGrabbed;

    /*
     * Classification of the gear
     *
     * 1 - Wood Gear
     * 2 - Iron Gear
     * 3 - Diamond Gear
     */

    int classification;
public:
    Gear()
    {
        x = 1;
        y = 1;
        classification = 0;
        bGrabbed = false;
    }

    void setX(int newX)
    {
        x = newX;
    }

    void setY(int newY)
    {
        y = newY;
    }

    int getX() const { return x; }

    int getY() { return y; }

    void setClassification(int newClass)
    {
        classification = newClass;
    }

    int getClassification() { return classification; }

    void grab() { bGrabbed = true; }

    void drop() { bGrabbed = false; }

    bool IsGrabbed() { return bGrabbed; }

};

class GameDemo : public olc::PixelGameEngine {
public:
    GameDemo()
    {
        // Name your application
        sAppName = "Sort the Gears";
        spawnTimer = 0;
        moveTimer = 1;
        score = 0;
        lives = 3;
        bGrabbed = false;
    }

private:

    std::vector<Gear> gears;

    bool bGrabbed;

    int score;
    int lives;
    int spawnTimer;
    int moveTimer;

    olc::vi2d vGearSize = {16 , 16};

    std::unique_ptr<olc::Sprite> sprTile;

    static int getGearSpawnPointX() {
        static std::random_device rd;
        static std::mt19937 eng(rd());
        std::uniform_int_distribution<int> distr(1, 14);
        return distr(eng);
    }

    static int getGearClassification() {
        static std::random_device rd;
        static std::mt19937 eng(rd());
        std::uniform_int_distribution<int> distr(1, 3);
        return distr(eng);
    }

public:
    bool OnUserCreate() override
    {
        // Called once at the start, so create things here
        sprTile = std::make_unique<olc::Sprite>("gfx/Gears-x16.png");
        gears = {};

        return true;
    }
    bool OnUserUpdate(float fElapsedTime) override {

        // Game Over condition
        if(lives == 0)
        {
            std::ofstream scoreFile;
            scoreFile.open("highscore.txt");
            scoreFile << std::to_string(score) + "\n";
            scoreFile.close();
            exit(score);
        }


        // Checks to make sure a gear is still being grabbed
        if(bGrabbed)
        {
            bool stillGrabbing = false;
            for (Gear& g : gears)
            {
                if(g.IsGrabbed())
                {
                    stillGrabbing = true;
                    break;
                }
            }
            if(!stillGrabbing)
            {
                bGrabbed = false;
            }
        }

        // Gear moving logic

        spawnTimer++;
        moveTimer++;

        if(GetMouse(0).bHeld && !bGrabbed)
        {
            int x = GetMouseX() - 10;
            int y = GetMouseY() - 10;

            int converted_X = x / 16;
            int converted_Y = y / 16;

            for(Gear& g : gears)
            {
                if(converted_X == g.getX() && converted_Y == g.getY())
                {
                    bGrabbed = true;
                    g.grab();
                    break;
                }
            }
        }

        if(GetMouse(0).bHeld && bGrabbed)
        {
            int x = GetMouseX() - 10;

            int converted_X = x / 16;

            for(Gear& g : gears)
            {
                if(g.IsGrabbed() && converted_X > 0 && converted_X < 15)
                {
                    g.setX(converted_X);
                    break;
                }
            }
        }

        if(GetMouse(0).bReleased && bGrabbed)
        {
            for(Gear& g : gears )
            {
                if(g.IsGrabbed())
                {
                    g.drop();
                    bGrabbed = false;
                    break;
                }
            }
        }

        // Spawning timer + mechanism
        if(spawnTimer == 1)
        {
            Gear newGear;
            newGear.setX(getGearSpawnPointX());
            newGear.setClassification(getGearClassification());
            gears.push_back(newGear);
        }else if(spawnTimer == (300 - score^3) + 5)
        {
            spawnTimer = 0;
        }

        //Movement timer
        if(moveTimer == 1)
        {
            for(int i = 0; i < gears.size(); i++)
            {
                Gear& g = gears[i];
                int newY;
                switch(g.getClassification())
                {
                    case 1:
                        newY = g.getY() + 1;
                        break;
                    case 2:
                        newY = g.getY() + 2;
                        break;
                    case 3:
                        newY = g.getY() + 3;
                        break;
                }
                if (newY > 13) {

                    int gX = g.getX();

                    // Wood Gear 1 - 5
                    if (gX == 1 || gX == 2 || gX == 3 || gX == 4 || gX == 5)
                    {

                        if (g.getClassification() == 1)
                        {
                            score++;
                        }else{
                            lives--;
                        }
                    }
                    // Iron Gear 6 - 10
                    if (gX == 6 || gX == 7 || gX == 8 || gX == 9 || gX == 10)
                    {
                        if (g.getClassification() == 2)
                        {
                            score += 2;
                        }else{
                            lives--;
                        }
                    }

                    // Diamond Gear 11 - 14
                    if (gX == 11 || gX == 12 || gX == 13 || gX == 14)
                    {
                        if (g.getClassification() == 3)
                        {
                            score += 3;
                        }else{
                            lives--;
                        }
                    }

                    gears.erase(gears.begin() + i);
                }else{
                    g.setY(newY);
                }
            }
        }else if(moveTimer == 100)
        {
            moveTimer = 0;
        }

        // Erase previous frame
        Clear(olc::DARK_BLUE);

        // Draw Boundary
        DrawLine(ScreenWidth() - 10, 10, ScreenWidth() - 10, ScreenHeight() - 10, olc::YELLOW);     // Left
        DrawLine(10, 10, 10, ScreenHeight() - 10, olc::YELLOW);                                     // Right
        DrawLine(10, ScreenHeight() - 10, ScreenWidth() - 10, ScreenHeight() - 10, olc::YELLOW);    // Bottom
        DrawLine(10, 10, ScreenWidth() - 10, 10, olc::YELLOW);                                      // Top

        // Draw score and lives remaining
        DrawString(1, 1, "Lives: " + std::to_string(lives));
        std::string scoreString = std::to_string(score);
        int scoreLength = scoreString.length();
        DrawString(ScreenWidth() - (scoreLength * 8) - 1, 1, scoreString);

        // Draw Gears

        SetPixelMode(olc::Pixel::MASK); // Don't draw pixels which have transparency

        for(Gear& g : gears)
        {
            switch(g.getClassification())
            {
                case 1:
                    DrawPartialSprite(olc::vi2d(g.getX(), g.getY()) * vGearSize, sprTile.get(), olc::vi2d(0, 0) * vGearSize, vGearSize);
                    break;
                case 2:
                    DrawPartialSprite(olc::vi2d(g.getX(), g.getY()) * vGearSize, sprTile.get(), olc::vi2d(1, 0) * vGearSize, vGearSize);
                    break;
                case 3:
                    DrawPartialSprite(olc::vi2d(g.getX(), g.getY()) * vGearSize, sprTile.get(), olc::vi2d(2, 0) * vGearSize, vGearSize);
                    break;
                default:
                    break;
            }
        }

        SetPixelMode(olc::Pixel::NORMAL); // Draw all pixels

        // Draw Gear Containers
        DrawRect(11, ScreenHeight() - 61, ((ScreenWidth() - 11) / 3) - 3, 50, olc::Colors::VERY_DARK_BROWN);
        FillRect(11, ScreenHeight() - 61, ((ScreenWidth() - 11) / 3) - 3, 50, olc::Colors::VERY_DARK_BROWN);
        DrawRect(((ScreenWidth() - 11) / 3) + 9, ScreenHeight() - 61, ((ScreenWidth() - 11) / 3) - 3, 50, olc::WHITE);
        FillRect(((ScreenWidth() - 11) / 3) + 9, ScreenHeight() - 61, ((ScreenWidth() - 11) / 3) - 3, 50, olc::WHITE);
        DrawRect((2 * ((ScreenWidth() - 11) / 3)) + 7, ScreenHeight() - 61, ((ScreenWidth() - 11) / 3) - 5, 50, olc::DARK_CYAN);
        FillRect((2 * ((ScreenWidth() - 11) / 3)) + 7, ScreenHeight() - 61, ((ScreenWidth() - 11) / 3) - 5, 50, olc::DARK_CYAN);

        return true;
    }
};

int main()
{
    GameDemo demo;
    if(demo.Construct(256, 240, 4, 4, false, true))
        demo.Start();
    return 0;
}