#include "shield.hpp"
#include "pixelcollision.hpp"
#include "floating.hpp"
#include "fireball.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <vector>

using namespace sf;
using namespace std;

int main()
{
    srand(static_cast<unsigned>(time(0)));

    const int windowwidth = 400;
    const int windowheight = 533;
    RenderWindow window(VideoMode({windowwidth, windowheight}), "Leap UP");
    window.setFramerateLimit(60);

    const int platformCount = 7;
    const int platformWidth = 60;
    const int platformHeight = 15;
    const int playerWidth = 58;
    const int playerHeight = 83;
    const int fireballHeight = 30;
    const int fireballWidth = 30;

    // Load all platform textures separately
    Texture platformTexture;       // Normal and moving platforms
    Texture thornTexture;          // Thorn platforms
    Texture disappearingTexture;   // Disappearing platforms
    Texture playerTexture;
    Texture bgTexture;
    Texture fireballTexture;

    if (!platformTexture.loadFromFile("plat.png", false, IntRect({10,10},{platformWidth,platformHeight})) ||
        !thornTexture.loadFromFile("thorn.png", false, IntRect({0,0},{platformWidth,platformHeight})) ||
        !disappearingTexture.loadFromFile("disappearing.png", false, IntRect({0,0},{platformWidth,platformHeight})) ||
        !playerTexture.loadFromFile("player.png", false, IntRect({0,0},{playerWidth,playerHeight})) ||
        !bgTexture.loadFromFile("bg.png", false, IntRect({0,0},{windowwidth,windowheight})) ||
        !fireballTexture.loadFromFile("fireball.png", false, IntRect({0,0},{fireballWidth, fireballHeight})))
    {
        cout << "Error loading one or more textures" << endl;
        return -1;
    }

    const Image& image1 = platformTexture.copyToImage();
    const Image& image2 = playerTexture.copyToImage();
    const Image& image3 = bgTexture.copyToImage();
    const Image& image4 = fireballTexture.copyToImage();

    // Sprites for drawing platforms by type
    Sprite normalPlatformSprite(platformTexture);
    Sprite thornSprite(thornTexture);
    Sprite disappearingSprite(disappearingTexture);

    Sprite player(playerTexture);
    Sprite bg(bgTexture);
    Sprite fireballSprite(fireballTexture);

    vector<Platform*> platforms;

    // Platform spawning loop
    for (int i = 0; i < platformCount; i++) {
        float x = static_cast<float>(rand() % (windowwidth - platformWidth));
        float y = i * (windowheight / platformCount);

        int r = rand() % 100; // random number 0–99

        Platform* platPtr = nullptr;

        if (r < 70) {
            // Normal platform
            platPtr = new Platform(x, y, false, PlatformType::Normal);
        }
        else if (r < 90) {
            // Moving platform
            platPtr = new Platform(x, y, true, PlatformType::Moving);
        }
        else if (r < 95) {
            // Thorn platform
            platPtr = new ThornPlatform(x, y);
        }
        else {
            // Disappearing platform
            platPtr = new DisappearingPlatform(x, y);
        }

        platforms.push_back(platPtr);
    }

    // Player origin centered
    player.setOrigin({playerWidth / 2.0f, playerHeight / 2.0f});

    const float movespeed = 3.f;
    const float playerJumpSpeed = 10.f;
    const float gravity = 0.2f;
    float a, b, h = 200; // a = x, b = y
    float da = 0, db = 0;

    // Start player on middle platform or fallback
    int midIndex = platformCount / 2;
    Platform* startPlatform = platforms[midIndex];

    if (!startPlatform->isVisible() || startPlatform->type == PlatformType::Thorn) {
        for (auto* plat : platforms) {
            if (plat->isVisible() && plat->type != PlatformType::Thorn) {
                startPlatform = plat;
                break;
            }
        }
    }

    a = startPlatform->position.x + platformWidth / 2.f;
    b = startPlatform->position.y - playerHeight / 2.f;

    Fireball fireball;
    Clock fireballTimer;

    Shield shield;
    shield.load(); // Load shield texture
    Clock shieldSpawnTimer;
    Clock shieldActiveTimer;
    Clock shieldLifetimeTimer; // For auto-despawn
    bool isShielded = false;
    float shieldWidth = shield.texture.getSize().x * shield.scaleFactor;
    float shieldHeight = shield.texture.getSize().y * shield.scaleFactor;

    CircleShape shieldAura;
    shieldAura.setRadius(50.f);
    shieldAura.setFillColor(Color(0, 0, 255, 100));
    shieldAura.setOrigin({shieldAura.getRadius(), shieldAura.getRadius()});

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }
        }

        // Update platforms
        for (auto* plat : platforms) {
            plat->update(windowwidth, platformWidth);
        }

        // Fireball spawning logic (unchanged)...

        // Movement input
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) a += movespeed;
        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) a -= movespeed;

        // Gravity and fall
        db += gravity;
        b += db;

        // Game Over if player falls below screen
        if (b - playerHeight / 2.f > window.getSize().y)
        {
            cout << "Game Over!" << endl;
            window.close();
        }

        // Scroll world logic (unchanged)...

        // Fireball update (unchanged)...

        // Player bounds
        float playerLeft = a - playerWidth / 2.f;
        float playerRight = a + playerWidth / 2.f;
        float playerTop = b - playerHeight / 2.f;
        float playerBottom = b + playerHeight / 2.f;

        // Shield spawn logic (unchanged)...

        // Shield active duration (unchanged)...

        // Fireball and shield aura collision (unchanged)...

        // Fireball and player collision (unchanged)...

        // Platform collision with player
        for (auto* plat : platforms) {
            if (!plat->isVisible()) continue;

            Vector2f platPos = plat->position;

            float platLeft = platPos.x;
            float platRight = platPos.x + platformWidth;
            float platTop = platPos.y;
            float platBottom = platPos.y + platformHeight;

            if (playerRight > platLeft && playerLeft < platRight &&
                playerBottom > platTop && playerTop < platBottom)
            {
                if (plat->type == PlatformType::Thorn) {
                    cout << "Game Over! Stepped on thorns." << endl;
                    window.close();
                    break;
                }
                else if (db > 0)
                {
                    float intersectionLeft = std::max(playerLeft, platLeft);
                    float intersectionTop = std::max(playerTop, platTop);
                    float intersectionRight = std::min(playerRight, platRight);
                    float intersectionBottom = std::min(playerBottom, platBottom);
                    float intersectionWidth = intersectionRight - intersectionLeft;
                    float intersectionHeight = intersectionBottom - intersectionTop;

                    // Choose correct sprite for collision test
                    Sprite* collisionSprite = nullptr;
                    switch (plat->type) {
                        case PlatformType::Normal:
                        case PlatformType::Moving:
                            collisionSprite = &normalPlatformSprite;
                            break;
                        case PlatformType::Disappearing:
                            collisionSprite = &disappearingSprite;
                            break;
                        default:
                            collisionSprite = &normalPlatformSprite;
                    }

                    collisionSprite->setPosition(platPos);
                    if (PerfectPixelCollision(player, image2, *collisionSprite, image1,
                                              intersectionLeft, intersectionTop,
                                              intersectionWidth, intersectionHeight))
                    {
                        b = platTop - playerHeight / 2.f;
                        db = -playerJumpSpeed;
                    }
                }
            }
        }

        // Update player position
        player.setPosition({a, b});

        // Shield pickup collision
        if (shield.isActive && shield.getBounds().findIntersection(player.getGlobalBounds())) {
            shield.deactivate();
            isShielded = true;
            shieldActiveTimer.restart();
        }

        // Despawn shield if ignored
        if (shield.isActive && shieldLifetimeTimer.getElapsedTime().asSeconds() > 10.f) {
            shield.deactivate();
        }

        // Draw
        window.clear();
        window.draw(bg);

        // Draw platforms with correct sprite by type
        for (auto* plat : platforms) {
            if (!plat->isVisible()) continue;

            switch (plat->type) {
                case PlatformType::Normal:
                case PlatformType::Moving:
                    normalPlatformSprite.setPosition(plat->position);
                    window.draw(normalPlatformSprite);
                    break;
                case PlatformType::Thorn:
                    thornSprite.setPosition(plat->position);
                    window.draw(thornSprite);
                    break;
                case PlatformType::Disappearing:
                    disappearingSprite.setPosition(plat->position);
                    window.draw(disappearingSprite);
                    break;
            }
        }

        // Draw shield
        if (shield.isActive) {
            shield.draw(window);
        }

        // Draw fireball if active
        if (fireball.isActive) {
            fireballSprite.setPosition(fireball.position);
            window.draw(fireballSprite);
        }

        // Draw player
        window.draw(player);

        // Draw shield aura if shielded
        if (isShielded) {
            shieldAura.setPosition(player.getPosition());
            window.draw(shieldAura);
        }

        window.display();
    }

    // Cleanup platform pointers
    for (auto* plat : platforms) {
        delete plat;
    }
    platforms.clear();

    return 0;
}
