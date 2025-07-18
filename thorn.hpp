#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class Thorn {
public:
    Vector2f position;
    Texture texture;
    bool isActive = false;
    float scale = 0.1f;

    Thorn()
        : position(-100.f, -100.f), isActive(false) {}

    bool load() {
        return texture.loadFromFile("thorn.png");
    }

    void spawn(float x, float y) {
        position = {x, y};
        isActive = true;
    }

    void deactivate() {
        isActive = false;
        position = {-100.f, -100.f};
    }

    void draw(RenderWindow& window) {
        if (!isActive) return;

        Sprite sprite(texture);
        sprite.setScale({scale, scale});
        sprite.setPosition(position);
        window.draw(sprite);
    }

    FloatRect getBounds() const {
        Sprite sprite(texture);
        sprite.setScale({scale, scale});
        sprite.setPosition(position);
        return sprite.getGlobalBounds();
    }
};
