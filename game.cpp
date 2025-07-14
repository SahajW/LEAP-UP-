#include "game.hpp"
#include "pixelcollision.hpp"
#include "floating.hpp"
#include "fireball.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <vector>

using namespace sf;
using namespace std;

void runGame(RenderWindow &window)
{
    srand(static_cast<unsigned>(time(0)));

    const int windowwidth = 1000;
    const int windowheight = 1400;
    window.setFramerateLimit(60);

    const int platformCount = 20;
    const int platformWidth = 100;
    const int platformHeight = 25;
    const int playerWidth = 60;
    const int playerHeight = 75;
    const int fireballHeight = 30;
    const int fireballWidth = 30;
    const float gameWidth = 1000.f;
    const float gameHeight = 1600.f;
    Texture plattformtexture, playerright, playerleft, bgtexture, fireballtexture;

    !plattformtexture.loadFromFile("assets/plat.png", false, IntRect({10, 10}, {platformWidth, platformHeight})) ||
        !playerright.loadFromFile("assets/player.png", false, IntRect({0, 0}, {playerWidth, playerHeight})) ||
        !playerleft.loadFromFile("assets/player2.png", false, IntRect({0, 0}, {playerWidth, playerHeight})) ||
        !bgtexture.loadFromFile("assets/bg1.png", false, IntRect({0, 0}, {windowwidth, windowheight})) ||
        !fireballtexture.loadFromFile("assets/fireball.png", false, IntRect({0, 0}, {fireballWidth, fireballHeight}));

    const Image &image1 = plattformtexture.copyToImage();
    const Image &image2 = playerright.copyToImage();
    const Image &image3 = playerleft.copyToImage();
    const Image &image4 = bgtexture.copyToImage();
    const Image &image5 = fireballtexture.copyToImage();

    Sprite platform(plattformtexture), player(playerright), bg(bgtexture), fireballsprite(fireballtexture);
    bool facingRight = true;

    vector<Platform> platforms;

    for (int i = 0; i < platformCount; i++)
    {
        float x = static_cast<float>(rand() % (windowwidth - platformWidth)); // Keep inside screen width
        float y = i * (windowheight / platformCount);                         // evenly spaced
        bool isMoving = rand() % 100 < 30;                                    // 30% chance to be moving
        platforms.emplace_back(x, y, isMoving);
    }
    // Player
    player.setOrigin({15.f, 15.f}); // Center the player sprite

    const float movespeed = 3.f;
    const float playerJumpSpeed = 10.f;
    const float gravity = 0.2f;
    float a, b, h = 500; // a = x, b = y
    float da = 0, db = 0;

    // Start player on middle platform
    int midIndex = platformCount / 2;
    float platformX = platforms[midIndex].position.x;
    float platformY = platforms[midIndex].position.y;

    a = platformX + platformWidth / 2.f; // horizontally centered
    b = platformY - playerHeight / 2.f;  // standing on top

    Fireball fireball;
    Clock fireballTimer;

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right)))
        {
            if (!facingRight)
            {
                player.setTexture(playerright);
                facingRight = true;
            }
            player.move({5.f, 0.f}); // move right
        }

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left)))
        {
            if (facingRight)
            {
                player.setTexture(playerleft);
                facingRight = false;
            }
            player.move({-5.f, 0.f}); // move left
        }

        for (auto &plat : platforms)
        {
            plat.update(windowwidth, platformWidth);
        }
        // Firball
        if (!fireball.isActive && fireballTimer.getElapsedTime().asSeconds() > 5.f)
        {
            vector<float> gaps;

            for (int i = 1; i < platforms.size(); ++i)
            {
                float upper = platforms[i - 1].position.y + platformHeight;
                float lower = platforms[i].position.y;

                if (lower - upper > fireballHeight + 10.f)
                {
                    float centerY = upper + (lower - upper) / 2.f - fireballHeight / 2.f;
                    gaps.push_back(centerY);
                }
            }

            if (!gaps.empty())
            {
                int index = rand() % gaps.size();
                fireball.spawn(gaps[index]);
                fireballTimer.restart();
            }
        }

        // Movement input
        if (Keyboard::isKeyPressed(Keyboard::Key::Right))
            a += movespeed;
        if (Keyboard::isKeyPressed(Keyboard::Key::Left))
            a -= movespeed;

        // Gravity and fall
        db += gravity;
        b += db;

        // Game Over if player falls below screen
        if (b - playerHeight / 2.f > window.getSize().y)
        {
            cout << "Game Over!" << endl;
            window.close();
        }

        // Scroll world if above height
        if (b < h)
        {
            b = h;
            for (int i = 0; i < platformCount; ++i)
            {
                platforms[i].position.y += -db;
                if (platforms[i].position.y > window.getSize().y)
                {
                    platforms[i].position.y = 0;
                    platforms[i].position.x = rand() % (window.getSize().x - static_cast<int>(platformWidth));
                }
            }
            if (fireball.isActive)
            {
                fireball.position.y += -db;
            }
        }

        // Fireball update
        fireball.update(windowwidth);

        // Player bounds
        float playerLeft = a - playerWidth / 2.f;
        float playerRight = a + playerWidth / 2.f;
        float playerTop = b - playerHeight / 2.f;
        float playerBottom = b + playerHeight / 2.f;

        // Check collision with fireball
        if (fireball.isActive && fireball.getBounds(fireballWidth, fireballHeight).findIntersection(player.getGlobalBounds()))
        {
            float intersectionLeft = std::max(playerLeft, fireball.position.x);
            float intersectionTop = std::max(playerTop, fireball.position.y);
            float intersectionRight = std::min(playerRight, fireball.position.x + fireballWidth);
            float intersectionBottom = std::min(playerBottom, fireball.position.y + fireballHeight);

            float intersectionWidth = intersectionRight - intersectionLeft;
            float intersectionHeight = intersectionBottom - intersectionTop;

            fireballsprite.setPosition(fireball.position); // update position before test

            if (PerfectPixelCollision(player, image2, fireballsprite, image4,
                                      intersectionLeft, intersectionTop,
                                      intersectionWidth, intersectionHeight))
            {
                cout << "Hit by fireball!" << endl;
                window.close();
            }
        }
        // Collision with platforms
        for (int i = 0; i < platformCount; ++i)
        {
            Vector2f platPos = platforms[i].position;

            float platLeft = platPos.x;
            float platRight = platPos.x + platformWidth;
            float platTop = platPos.y;
            float platBottom = platPos.y + platformHeight;

            if (playerRight > platLeft &&
                playerLeft < platRight &&
                playerBottom > platTop &&
                playerTop < platBottom &&
                db > 0) // Falling
            {
                float intersectionLeft = std::max(playerLeft, platLeft);
                float intersectionTop = std::max(playerTop, platTop);
                float intersectionRight = std::min(playerRight, platRight);
                float intersectionBottom = std::min(playerBottom, platBottom);
                float intersectionWidth = intersectionRight - intersectionLeft;
                float intersectionHeight = intersectionBottom - intersectionTop;

                platform.setPosition(platPos); // platform sprite must match this platform
                if (PerfectPixelCollision(player, image2, platform, image1, intersectionLeft, intersectionTop, intersectionWidth, intersectionHeight))
                {
                    db = -playerJumpSpeed;
                    b = platPos.y - playerHeight / 2.f; // Jump back up
                }
            }
        }
        if (playerWidth> gameWidth + playerWidth/2.f)
        {
            a = 0; // Reappear just inside left edge
            b = playerHeight;
        }
        else if (playerWidth< 0)
        {
            a = gameWidth; // Reappear just inside right edge
            b = playerHeight;
        }
        player.setPosition({a, b});

        // Draw
        window.clear();
        window.draw(bg);
        for (const auto &plat : platforms)
        {
            platform.setPosition(plat.position);
            window.draw(platform);
        }
        if (fireball.isActive)
        {
            fireballsprite.setPosition(fireball.position);
            window.draw(fireballsprite);
        }

        window.draw(player);
        window.display();
    }
}