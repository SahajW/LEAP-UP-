#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class jetpack {
public:
    Vector2f position;
    bool isActive; //isActive refers to the shield powerup shown on the screen before the player takes it
    Texture texture;
    

    jetpack()
        : position(-100.f, -100.f), isActive(false) {
    }

    void load() {
        if (!texture.loadFromFile("assets/jetpackicon.png")) {
            throw std::runtime_error("Failed to load jetpack.png");
        }
    }

    // Spawn above a platform or between them
    void spawn(float x, float y) {
        position = { x, y };
        isActive = true;
    }

    void deactivate() {
        isActive = false;
        position = { -100.f, -100.f };
    }

    void draw(RenderWindow& window) const {
        if (isActive) {
            Sprite tempSprite(texture);
            tempSprite.setPosition(position);
            window.draw(tempSprite);
        }
    }

    FloatRect getBounds() const {
        Sprite tempSprite(texture);
        
        tempSprite.setPosition(position);
        return tempSprite.getGlobalBounds();
    }
};