// Player.hpp
#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Player {
public:
    sf::Sprite sprite;
    sf::Vector2f position;
    float width, height;
    float velocityY = 0.f;
    float gravity = 0.2f;
    float jumpSpeed = 10.f;
    const sf::Texture& texture;  //  Store texture as reference

    Player(const sf::Texture& texture, float w, float h);
    void update();
    void move(float dx);
    void applyGravity();
    void jump();
    void setPosition(float x, float y);
    sf::FloatRect getBounds() const;
};
