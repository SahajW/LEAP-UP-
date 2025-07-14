#include "player.hpp"
#include <iostream>

Player::Player() 
: sprite(idleTexture)
{
    if (!idleTexture.loadFromFile("assets/Jump.png")) {
        std::cerr << "Failed to load idle texture\n";
    }

    // Load jump textures
    for (int i = 2; i <= 3; ++i) {
        sf::Texture tex;
        if (!tex.loadFromFile("assets/Jump" + std::to_string(i) + ".png")) {
            std::cerr << "Failed to load jump frame " << i << "\n";
        } else {
            jumpTextures.push_back(std::move(tex));
        }
    }

    sprite.setTexture(idleTexture);
    sprite.setScale({3.f, 3.f});
    sprite.setPosition({400.f, 1200.f});

    velocity = {0.f, jumpSpeed}; // Start jumping
}

void Player::update(float deltaTime) {
    velocity.y += gravity * deltaTime;
    sprite.move(sf::Vector2f(0.f, velocity.y * deltaTime));


    animate(deltaTime);
}

void Player::animate(float deltaTime) {
    if (velocity.y < 0.f) { // Jumping up
        animationTimer += deltaTime;
        if (animationTimer >= 0.12f) {
            animationTimer = 0.f;
            currentFrame = (currentFrame + 1) % jumpTextures.size();
            sprite.setTexture(jumpTextures[currentFrame]);
        }
    } else {
        sprite.setTexture(idleTexture);
        currentFrame = 0;
        animationTimer = 0.f;
    }
}

void Player::jump() {
    velocity.y = jumpSpeed;
}

void Player::draw(sf::RenderWindow& window) const {
    window.draw(sprite);
}

sf::FloatRect Player::getBounds() const {
    return sprite.getGlobalBounds();
}

sf::Vector2f Player::getPosition() const {
    return sprite.getPosition();
}

void Player::setPosition(sf::Vector2f pos) {
    sprite.setPosition(pos);
}
