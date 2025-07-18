#pragma once
#include <SFML/Graphics.hpp>
using namespace sf;

class DisappearingPlatform {
public:
    Vector2f position;
    bool isVisible = true;
    float timer = 0.f;
    float disappearTime = 15.f; // frames

    DisappearingPlatform(float x, float y) : position(x, y) {}

    void update() {
        if (!isVisible) return;
        if (timer > 0) {
            timer -= 1.f;
            if (timer <= 0) {
                isVisible = false;
            }
        }
    }

    void trigger() {
        timer = disappearTime;
    }

    FloatRect getBounds(float width, float height) const {
        return FloatRect(position, {width, height});
    }
};
