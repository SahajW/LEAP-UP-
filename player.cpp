// Player.cpp
#include "Player.hpp"
using namespace sf;
Player::Player(const sf::Texture& texture, float w, float h)
    : width(w), height(h), texture(texture) {  //  initialize reference
    sprite.setTexture(texture);
    sprite.setOrigin({w / 2.f, h / 2.f});
}


void Player::update() {
    position.y += velocityY;
    sprite.setPosition(position);
}

void Player::move(float dx) {
    position.x += dx;
}

void Player::applyGravity() {
    velocityY += gravity;
}

void Player::jump() {
    velocityY = -jumpSpeed;
}

void Player::setPosition(float x, float y) {
    position = {x, y};
    sprite.setPosition(position);
}

sf::FloatRect Player::getBounds() const {
    return sprite.getGlobalBounds();
}
