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

        Fireball()
        {
            position = {-100.f, -100.f}; // Start off-screen
            speed = 7.f; // Default speed
            isActive = false; // Initially inactive
        }
        void spawn(float y)
        {
            position={-30.f, y}; // Spawn off-screen to the left
            isActive = true; // Activate the fireball
        }
        void update(float windowWidth)
        {
            if (!isActive) return;
            if(isActive)
            {
                position.x+=speed;
                if (position.x > windowWidth)
                {
                    isActive = false;
                    position = {-100.f,-100.f}; // Reset position off-screen
                }
            }
        }
    sf::FloatRect getBounds(float width, float height) const {
    return sf::FloatRect(position, sf::Vector2f(width, height));
}
};