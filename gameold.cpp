#include "game.hpp"
#include "shield.hpp"
#include "pixelcollision.hpp"
#include "floating.hpp"
#include "fireball.hpp"
#include <SFML/Graphics.hpp>
#include <fstream>
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
    const float gameHeight = 1400.f;
    int score = 0;
    float worldHeight = 0.f;

    int highscore = 0;
    ifstream inputFile("highscore.txt");
    if (inputFile.is_open())
    {
        inputFile >> highscore;
        inputFile.close();
    }

    sf::Font font;
    !font.openFromFile("assets/pixel.ttf");

    Text scoreText(font);
    scoreText.setFillColor(Color::White);
    scoreText.setStyle(Text::Bold);
    scoreText.setCharacterSize(36);
    scoreText.setPosition({10, 10});

    Text highscoreText(font);
    highscoreText.setFillColor(Color::White);
    highscoreText.setStyle(Text::Bold);
    highscoreText.setCharacterSize(36);
    highscoreText.setPosition({10, 45});
    highscoreText.setString("High Score: " + to_string(highscore));

    Texture plattformtexture, playerright, playerleft, bgtexture, fireballtexture, pauseTexture, resumeTexture,
        QuitTexture, resumeTexture2, QuitTexture2;

    !plattformtexture.loadFromFile("assets/plat.png", false, IntRect({10, 10}, {platformWidth, platformHeight})) ||
        !playerright.loadFromFile("assets/player.png", false, IntRect({0, 0}, {playerWidth, playerHeight})) ||
        !playerleft.loadFromFile("assets/player2.png", false, IntRect({0, 0}, {playerWidth, playerHeight})) ||
        !bgtexture.loadFromFile("assets/bg1.png", false, IntRect({0, 0}, {windowwidth, windowheight})) ||
        !fireballtexture.loadFromFile("assets/fireball.png", false, IntRect({0, 0}, {fireballWidth, fireballHeight})) ||
        !pauseTexture.loadFromFile("assets/pause.png") ||
        !resumeTexture.loadFromFile("assets/resume.png") ||
        !QuitTexture.loadFromFile("assets/quit.png") ||
        !resumeTexture2.loadFromFile("assets/resume2.png") ||
        !QuitTexture2.loadFromFile("assets/exit2.png");

    const Image &image1 = plattformtexture.copyToImage();
    const Image &image2 = playerright.copyToImage();
    const Image &image3 = playerleft.copyToImage();
    const Image &image4 = bgtexture.copyToImage();
    const Image &image5 = fireballtexture.copyToImage();

    Sprite platform(plattformtexture), player(playerright), bg(bgtexture), fireballsprite(fireballtexture),
        pause(pauseTexture), resume(resumeTexture), quit(QuitTexture), resume2(resumeTexture2), quit2(QuitTexture2);
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
    player.setOrigin({playerWidth / 2.0f, playerHeight / 2.0f}); // Center the player sprite

    const float movespeed = 3.f;
    const float playerJumpSpeed = 10.f;
    const float gravity = 0.2f;
    float a, b, h = 200; // a = x, b = y
    float da = 0, db = 0;

    // Start player on middle platform
    int midIndex = platformCount / 2;
    float platformX = platforms[midIndex].position.x;
    float platformY = platforms[midIndex].position.y;

    a = platformX + platformWidth / 2.f; // horizontally centered
    b = platformY - playerHeight / 2.f;  // standing on top

    Fireball fireball;
    Clock fireballTimer;

    Shield shield;
    shield.load(); // Load texture
    Clock shieldSpawnTimer;
    Clock shieldActiveTimer;
    Clock shieldLifetimeTimer; // New timer to auto-despawn shield
    bool isShielded = false;
    float shieldWidth = shield.texture.getSize().x * shield.scaleFactor;  // Scaled width
    float shieldHeight = shield.texture.getSize().y * shield.scaleFactor; // Scaled height
    // draw shieldaura
    CircleShape shieldAura;
    shieldAura.setRadius(50.f);                     // Radius slightly larger than player
    shieldAura.setFillColor(Color(0, 0, 255, 100)); // Blue with transparency
    shieldAura.setOrigin({shieldAura.getRadius(), shieldAura.getRadius()});
    pause.setPosition({900.f, 0.f});
    pause.setScale({0.25f, 0.25f});
    quit.setPosition({300.f, 500.f});
    quit.setScale({0.5f, 0.5f});
    resume.setPosition({300.f, 350.f});
    resume.setScale({0.5f, 0.5f});

    bool isPaused = false;

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }
            else if (event->is<sf::Event::MouseButtonPressed>())
            {
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
                if (isPaused)
                {
                    // Handle clicks on resume and quit buttons
                    if (resume.getGlobalBounds().contains(mousePos))
                    {
                        isPaused = false;
                    }
                    else if (quit.getGlobalBounds().contains(mousePos))
                    {
                        return; // Return from runGame → back to main menu
                    }
                }
                else
                {
                    if (pause.getGlobalBounds().contains(mousePos))
                    {
                        isPaused = !isPaused;
                    }
                }
            }
            else if (event->is<sf::Event::MouseMoved>())
            {
                if (isPaused)
                {
                    Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                    if (resume.getGlobalBounds().contains(mousePos))
                        resume.setTexture(resumeTexture2);
                    else
                        resume.setTexture(resumeTexture);

                    if (quit.getGlobalBounds().contains(mousePos))
                        quit.setTexture(QuitTexture2);
                    else
                        quit.setTexture(QuitTexture);
                }
            }
            else if (event->is<Event::KeyPressed>())
            {
                if (Keyboard::isKeyPressed(Keyboard::Key::P))
                {
                    isPaused = !isPaused;
                }
            }
        }
        if (!isPaused)
        {

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
            if (!fireball.isActive && fireballTimer.getElapsedTime().asSeconds() > 3.f)
            {
                vector<float> gaps;
                // Find gaps between platforms
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
                // If there are gaps, spawn fireball in a random gap
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
            score = static_cast<int>(worldHeight / 50);        // <-- SCORE RELATED
            scoreText.setString("Score: " + to_string(score)); // <-- SCORE RELATED

            if (score > highscore)
            {
                highscore = score;
                highscoreText.setString("High Score: " + to_string(highscore));
            }

            // Scroll world if above height
            if (b < h)
            {
                int diff = h - b;
                b = h;
                worldHeight += diff; // Track how much we've scrolled
                for (int i = 0; i < platformCount; ++i)
                {
                    platforms[i].position.y += diff;
                    // platforms[i].position.y += -db;
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
                if (shield.isActive)
                {
                    shield.position.y += -db;
                }
            }

            // Fireball update
            fireball.update(windowwidth);

            // Player bounds
            float playerLeft = a - playerWidth / 2.f;
            float playerRight = a + playerWidth / 2.f;
            float playerTop = b - playerHeight / 2.f;
            float playerBottom = b + playerHeight / 2.f;

            // Spawn shield every 5-15 seconds
            if (!shield.isActive && !isShielded && shieldSpawnTimer.getElapsedTime().asSeconds() > (5 + rand() % 10))
            {
                // Collect valid platforms where y is between h and windowheight
                vector<int> validPlatformIndices;
                for (int i = 0; i < platformCount; ++i)
                {
                    float platY = platforms[i].position.y;
                    if (platY <= h && platY > 0.f)
                    {
                        validPlatformIndices.push_back(i);
                    }
                }

                if (!validPlatformIndices.empty())
                {
                    int randomIndex = validPlatformIndices[rand() % validPlatformIndices.size()];
                    float x = platforms[randomIndex].position.x + (platformWidth - shieldWidth) / 2.f;
                    float y = platforms[randomIndex].position.y - shieldHeight; // Just above platform
                    shield.spawn(x, y);

                    shieldLifetimeTimer.restart(); // Start auto-despawn timer here
                    shieldSpawnTimer.restart();    // Reset spawn timer
                }
            }

            // turn off the shield after 20 seconds
            if (isShielded && shieldActiveTimer.getElapsedTime().asSeconds() > 20.f)
            {
                isShielded = false;
            }

            // Check collision with shield aura
            if (fireball.isActive && isShielded)
            {
                FloatRect fireballBounds = fireball.getBounds(fireballWidth, fireballHeight);
                FloatRect auraBounds = shieldAura.getGlobalBounds();

                if (fireballBounds.findIntersection(auraBounds))
                {
                    // Collision with aura (cancels both)
                    fireball.isActive = false;
                    fireball.position = {-100.f, -100.f}; // Move offscreen
                    isShielded = false;
                    continue; // Skip further fireball collision checks this frame
                }
            }

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

                if (!isShielded && PerfectPixelCollision(player, image2, fireballsprite, image4,
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
                        b = platTop - playerHeight / 2.f; // Set player exactly on top
                        db = -playerJumpSpeed;            // Apply jump velocity
                    }
                }
            }
            // Update player position
            player.setPosition({a, b});
            // Check shield collision(if player picks up shield)
            if (shield.isActive && shield.getBounds().findIntersection(player.getGlobalBounds()))
            {
                shield.deactivate();         // Despawn
                isShielded = true;           // Activate shield
                shieldActiveTimer.restart(); // Start timer
            }
            // Despawn shield if ignored for too long
            if (shield.isActive && shieldLifetimeTimer.getElapsedTime().asSeconds() > 10.f)
            {
                shield.deactivate();
            }
        }
        // Draw
        window.clear();
        window.draw(bg);
        // Draw platforms
        for (const auto &plat : platforms)
        {
            platform.setPosition(plat.position);
            window.draw(platform);
        }
        // draw shield
        if (shield.isActive)
        {
            shield.draw(window);
        }
        // Draw fireball if active
        if (fireball.isActive)
        {
            fireballsprite.setPosition(fireball.position);
            window.draw(fireballsprite);
        }
        // draw shield aura if shielded

        // Draw player
        window.draw(player);
        window.draw(scoreText);
        window.draw(highscoreText);
        window.draw(pause);
        if (isPaused)
        {
            RectangleShape dim(Vector2f(windowwidth, windowheight));
            dim.setFillColor(Color(0, 0, 0, 150)); // semi-transparent black
            window.draw(dim);
            window.draw(resume);
            window.draw(quit);
        }
        if (isShielded)
        {
            shieldAura.setPosition(player.getPosition());
            window.draw(shieldAura);
        }
        window.display();
    }
    ofstream outputFile("highscore.txt");

    if (outputFile.is_open())
    {
        outputFile << highscore;
        outputFile.close();
    }
}