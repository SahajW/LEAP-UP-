#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <vector>

using namespace sf;
using namespace std;

class Platform
{
    public:
       Vector2f position;
       bool isMoving=false;
       float speed=0.f;
       bool movingLeft=true;

       Platform(float x, float y,bool moving=false) : position(x, y),isMoving(moving)
       {
              if(isMoving)
              {
                speed = 2.0f + static_cast<float>(rand() % 5); // Random speed between 1 and 3
                movingLeft = rand() % 2;
             }
     }
     void update(float windowWidth,float platformWidth)
     {
        if(!isMoving) return;
        if(movingLeft)
        {
            position.x -= speed;
            if(position.x < 0) // If it goes out of bounds
            {
                position.x = 0; // Reset to left edge
                movingLeft = false;
            }
        }
        else
        {
            position.x += speed;
            if(position.x + platformWidth > windowWidth) // If it goes out of bounds
            {
                position.x = windowWidth - platformWidth; // Reset to right edge
                movingLeft = true;
            }
        }
     }
};

