#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <vector>

using namespace sf;
using namespace std;

class Fireball
{
public:
    Vector2f position;
    float speed;
    bool isActive;
    bool moveRight; // New: determines direction (true = left->right, false = right->left)

    Fireball()
    {
        position = {-100.f, -100.f};  // Start off-screen
        isActive = false;            // Initially inactive
        moveRight = true;            // Default direction
    }

    // Spawn only above the player
    void spawn(float playerY, float windowWidth)
    {
        float minY = playerY - 150.f; // Spawn at least 150 pixels above the player
        if (minY < 0.f) minY = 0.f;
        
      

        float y = minY + static_cast<float>(rand() % 50); // Randomly a bit above
        moveRight = rand() % 2; // Randomly choose direction (0 = left, 1 = right)

        if (moveRight)
        {
            // Spawn from left going right
            position = {-30.f, y};
        }
        else
        {
            // Spawn from right going left
            position = {windowWidth + 30.f, y};
        }

        isActive = true;
    }

    void update(float windowWidth,float speed)
    {
        if (!isActive) return;

        if (moveRight)
        {
            position.x += speed;
            if (position.x > windowWidth + 50.f)
            {
                isActive = false;
                position = {-100.f, -100.f};
            }
        }
        else
        {
            position.x -= speed;
            if (position.x < -50.f)
            {
                isActive = false;
                position = {-100.f, -100.f};
            }
        }
    }

    // Get bounding box (collision)
    sf::FloatRect getBounds(float width, float height) const
    {
        return sf::FloatRect(position, sf::Vector2f(width, height));
    }
};