#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Player {
public:
    Player(); // Loads textures, sets up sprite

    void update(float deltaTime);               // Handles gravity + animation
    void jump();                                // Called when player hits platform
    void draw(sf::RenderWindow& window) const;  // Draw player
    sf::FloatRect getBounds() const;            // For collision
    sf::Vector2f getPosition() const;           // For scrolling
    void setPosition(sf::Vector2f pos);         // Reset pos if needed

private:
    sf::Sprite sprite;
    sf::Texture idleTexture;
    std::vector<sf::Texture> jumpTextures;

    float animationTimer = 0.f;
    int currentFrame = 0;

    sf::Vector2f velocity;
    const float gravity = 900.f;
    const float jumpSpeed = -600.f;

    void animate(float deltaTime);
};
