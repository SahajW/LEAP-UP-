#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

// Enum to distinguish platform types
enum class PlatformType
{
    Normal,
    Moving,
    Disappearing,
    Thorn
};

// Base platform class
class Platform
{
public:
    Vector2f position;
    bool isMoving = false;
    float speed = 0.f;
    bool movingLeft = true;
    PlatformType type = PlatformType::Normal;

    Platform(float x, float y, bool moving = false, PlatformType t = PlatformType::Normal,float baseSpeed=0.f)
        : position(x, y), isMoving(moving), type(t)
    {
        if (isMoving)
        {
            speed = baseSpeed + static_cast<float>(rand() % 3); //  2–4 or 4–6
        }
    }

    // Virtual destructor allows safe deletion via base pointer
    virtual ~Platform() = default;

    // Virtual update: overridden by subclasses if needed
    virtual void update(float windowWidth, float platformWidth)
    {
        if (!isMoving)
            return;

        if (movingLeft)
        {
            position.x -= speed;
            if (position.x < 0)
            {
                position.x = 0;
                movingLeft = false;
            }
        }
        else
        {
            position.x += speed;
            if (position.x + platformWidth > windowWidth)
            {
                position.x = windowWidth - platformWidth;
                movingLeft = true;
            }
        }
    }

    // Visibility check, overridden in DisappearingPlatform
    virtual bool isVisible() const
    {
        return true;
    }
};

// Platform that disappears after 2 seconds
class DisappearingPlatform : public Platform
{
private:
    bool visible = true;
    bool touched = false;
    bool jumpedAfterTouch = false;

public:
    DisappearingPlatform(float x, float y)
        : Platform(x, y, false, PlatformType::Disappearing)
    {
    }

    void update(float windowWidth, float platformWidth) override
    {
        Platform::update(windowWidth, platformWidth);

        // If player has touched and jumped afterward, hide
        if (touched && jumpedAfterTouch)
        {
            visible = false;
        }
    }

    void onPlayerTouch()
    {
        touched = true;
        jumpedAfterTouch = false; // reset if touched again
    }

    void onPlayerJump()
    {
        if (touched)
        {
            jumpedAfterTouch = true;
        }
    }

    bool isVisible() const override
    {
        return visible;
    }
};
// Platform that kills player on contact
class ThornPlatform : public Platform
{
public:
    ThornPlatform(float x, float y)
        : Platform(x, y, false, PlatformType::Thorn)
    {
        // No special behavior in update() (yet)
    }

};