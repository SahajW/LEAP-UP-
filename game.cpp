#include "shield.hpp"
#include "pixelcollision.hpp"
#include "floating.hpp"
#include "fireball.hpp"
#include "jetpack.hpp"
#include "game.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>

using namespace sf;
using namespace std;

void runGame(RenderWindow &window)
{
    VideoMode desktopMode = VideoMode::getDesktopMode();

    // Calculate window size as a percentage of screen size (e.g., 70% width, 60% height)
    const sf::Vector2u windowSize(
        static_cast<unsigned int>((desktopMode.size.x / 2.65f)),
        static_cast<unsigned int>((desktopMode.size.y / 1.142857143f)));
    srand(static_cast<unsigned>(time(0)));

    const int windowwidth = desktopMode.size.x / 2.56f;
    const int windowheight = desktopMode.size.y / 1.142857143f;

    window.setFramerateLimit(60);

    const int platformCount = 15;
    const int platformWidth = windowwidth / 7.815;
    const int platformHeight = windowheight / 66.666666667;
    const int playerWidth = windowwidth / 16.666666667;
    const int playerHeight = windowheight / 18.666666667;
    const int fireballWidth = windowwidth / 19.60784314;
    const int fireballHeight = windowheight / 40;

    int score = 0;
    float worldHeight = 0.f;

    int lastShieldScore = -1000;
    int lastJetpackScore = -1000;
    const int shieldInterval = 50;   // spawn every 50 score
    const int jetpackInterval = 200; // spawn every 200 score

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
    scoreText.setPosition({windowwidth / 100.f, windowheight / 140.f});

    Text highscoreText(font);
    highscoreText.setFillColor(Color::White);
    highscoreText.setStyle(Text::Bold);
    highscoreText.setCharacterSize(36);
    highscoreText.setPosition({windowwidth / 100.f, windowheight / 31.111111111f});
    highscoreText.setString("High Score: " + to_string(highscore));

    // Load platform textures separately
    Texture platformTexture, thornTexture, disappearingTexture, playerright, playerleft, bgTexture, fireballTexture, jplayertexture,
        pauseTexture, resumeTexture,
        QuitTexture, resumeTexture2, QuitTexture2, GameOver, TryAgain, TryAgain2;

    !platformTexture.loadFromFile("assets/plat1.png", false, IntRect({0, 0}, {platformWidth, platformHeight})) ||
        !thornTexture.loadFromFile("assets/plat3.png", false, IntRect({0, 0}, {platformWidth, platformHeight})) ||
        !disappearingTexture.loadFromFile("assets/plat2.png", false, IntRect({0, 0}, {platformWidth, platformHeight})) ||
        !playerright.loadFromFile("assets/player.png", false, IntRect({0, 0}, {playerWidth, playerHeight})) ||
        !playerleft.loadFromFile("assets/player2.png", false, IntRect({0, 0}, {playerWidth, playerHeight})) ||
        !bgTexture.loadFromFile("assets/bg1.png", false, IntRect({0, 0}, {windowwidth, windowheight})) ||
        !fireballTexture.loadFromFile("assets/fireball.png", false, IntRect({0, 0}, {fireballWidth, fireballHeight})) ||
        !jplayertexture.loadFromFile("assets/jetpackplayer.png", false, IntRect({0, 0}, {playerWidth, playerHeight})) ||
        !pauseTexture.loadFromFile("assets/pause.png") ||
        !resumeTexture.loadFromFile("assets/resume.png") ||
        !QuitTexture.loadFromFile("assets/quit.png") ||
        !resumeTexture2.loadFromFile("assets/resume2.png") ||
        !QuitTexture2.loadFromFile("assets/exit2.png") ||
        !GameOver.loadFromFile("assets/over.png") ||
        !TryAgain.loadFromFile("assets/again.png") ||
        !TryAgain2.loadFromFile("assets/again2.png");

    const Image &image1 = platformTexture.copyToImage();
    const Image &image2 = playerright.copyToImage();
    const Image &image3 = playerleft.copyToImage();
    const Image &image4 = bgTexture.copyToImage();
    const Image &image5 = fireballTexture.copyToImage();

    Sprite normalPlatformSprite(platformTexture);
    Sprite thornSprite(thornTexture);
    Sprite disappearingSprite(disappearingTexture);

    Sprite player(playerright), bg(bgTexture), fireballSprite(fireballTexture), jp(jplayertexture),
        pause(pauseTexture), resume(resumeTexture), quit(QuitTexture), resume2(resumeTexture2),
        quit2(QuitTexture2), over(GameOver), again(TryAgain), again2(TryAgain2);
    bool facingRight = true;

    jp.setOrigin({playerWidth / 2.f, playerHeight / 2.f}); // setting its scale to center

    // platform creation
    vector<Platform *> platforms;

    float verticalSpacing = windowheight / static_cast<float>(platformCount);
    float currentY = windowheight - verticalSpacing;

    for (int i = 0; i < platformCount; ++i)
    {
        float x = static_cast<float>(rand() % (windowwidth - platformWidth));
        float y = currentY;

        Platform *plat = nullptr;

        int r = rand() % 100;
        if (r < 70)
            plat = new Platform(x, y, false, PlatformType::Normal);
        else if (r < 90)
            plat = new Platform(x, y, true, PlatformType::Moving, static_cast<float>(rand() % 3 + 2));
        else if (r < 95)
            plat = new ThornPlatform(x, y);
        else
            plat = new DisappearingPlatform(x, y);

        platforms.push_back(plat);
        currentY -= verticalSpacing; // evenly go upward
    }

    player.setOrigin({playerWidth / 2.f, playerHeight / 2.f});

    const float movespeed = 5.f;
    const float playerJumpSpeed = 12.f;
    const float gravity = 0.2f;
    float a, b, h = 500; // a=x, b=y
    float da = 0, db = 0;

    int midIndex = platformCount / 2;
    Platform *startPlatform = platforms[midIndex];

    if (!startPlatform->isVisible() || startPlatform->type == PlatformType::Thorn)
    {
        for (auto *plat : platforms)
        {
            if (plat->isVisible() && plat->type != PlatformType::Thorn)
            {
                startPlatform = plat;
                break;
            }
        }
    }

    a = startPlatform->position.x + platformWidth / 2.f;
    b = startPlatform->position.y - playerHeight / 2.f;

    // --- Jetpack Integration Start ---
    jetpack jetpackItem;
    try
    {
        jetpackItem.load(); // Load jetpack texture
    }
    catch (const std::runtime_error &e)
    {
        cout << e.what() << endl;
        return;
    }

    Clock jetpackActiveTimer;
    Clock jetpackLifetimeTimer; // New timer to auto-despawn jetpack
    bool isJetpacked = false;
    const float jetpackBoostSpeed = -8.0f; // Stronger upward velocity when jetpacked

    Clock jetpackEffectCooldownTimer;
    const float jetpackEffectCooldownDuration = 5.0f; // For example, 5 seconds cooldown
    // Initialize to a high value so it doesn't block initial spawn
    jetpackEffectCooldownTimer.restart(); // Will be reset when jetpack effect ends
    // --- Jetpack Integration End ---

    Fireball fireball;
    Clock fireballTimer;

    Shield shield;
    shield.load();
    Clock shieldActiveTimer;
    bool isShielded = false;
    float shieldWidth = shield.texture.getSize().x;
    float shieldHeight = shield.texture.getSize().y;

    CircleShape shieldAura;
    shieldAura.setRadius(50.f);
    shieldAura.setFillColor(Color(0, 0, 255, 100));
    shieldAura.setOrigin({shieldAura.getRadius(), shieldAura.getRadius()});

    pause.setPosition({windowwidth / 1.142857143f, 0.f});
    pause.setScale({windowwidth / 4000.f, windowheight / 5600.f});

    quit.setPosition({windowwidth / 3.333333333f, windowheight / 2.8f});
    quit.setScale({windowwidth / 2000.f, windowheight / 2800.f});

    over.setPosition({windowwidth / 6.666666667f, windowheight / 3.5f});
    over.setScale({windowwidth / 1428.571429f, windowheight / 2000.f});

    again.setPosition({windowwidth / 3.333333333f, windowheight / 2.666666667f});
    again.setScale({windowwidth / 2000.f, windowheight / 2800.f});

    resume.setPosition({windowwidth / 3.333333333f, windowheight / 4.f});
    resume.setScale({windowwidth / 2000.f, windowheight / 2800.f});

    bool isPaused = false;
    bool isGameOver = false;

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

                // Game Over: Check for Try Again click
                if (isGameOver)
                {
                    if (again.getGlobalBounds().contains(mousePos))
                    {
                        return; // Exits runGame(); you can call runGame(window) again from main
                    }
                }
                else if (isPaused)
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
                Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

                if (isPaused)
                {
                    if (resume.getGlobalBounds().contains(mousePos))
                        resume.setTexture(resumeTexture2);
                    else
                        resume.setTexture(resumeTexture);

                    if (quit.getGlobalBounds().contains(mousePos))
                        quit.setTexture(QuitTexture2);
                    else
                        quit.setTexture(QuitTexture);
                }

                if (isGameOver)
                {
                    if (again.getGlobalBounds().contains(mousePos))
                        again.setTexture(TryAgain2);
                    else
                        again.setTexture(TryAgain);
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

        if (!isPaused && !isGameOver)
        {
            score = static_cast<int>(worldHeight / 50);
            scoreText.setString("Score: " + to_string(score));
            bool hardMode = (score >= 200); // Hard mode starts at score 200

            float gravity = hardMode ? 0.22f : 0.2f;
            float playerJumpSpeed = hardMode ? 13.f : 12.f;

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

            // Update platforms
            for (auto *plat : platforms)
            {
                plat->update(windowwidth, platformWidth);
            }

            // Fireball spawn logic
            if (!fireball.isActive && fireballTimer.getElapsedTime().asSeconds() > 3.f)
            {
                vector<float> gaps;
                for (size_t i = 1; i < platforms.size(); ++i)
                {
                    Platform *prevPlat = platforms[i - 1];
                    Platform *currPlat = platforms[i];

                    if (!prevPlat->isVisible() || !currPlat->isVisible())
                        continue;

                    if (prevPlat->type == PlatformType::Thorn || currPlat->type == PlatformType::Thorn)
                        continue;

                    float upper = prevPlat->position.y + platformHeight;
                    float lower = currPlat->position.y;

                    if (lower - upper > fireballHeight + 10.f)
                    {
                        float centerY = upper + (lower - upper) / 2.f - fireballHeight / 2.f;
                        float verticalVelocity = db; // Or however you track jump/gravity
                        float futurePlayerY = player.getPosition().y + verticalVelocity * 15.f;

                        if (centerY < futurePlayerY)
                        {
                            gaps.push_back(centerY);
                        }
                    }
                }

                if (!gaps.empty())
                {
                    int index = rand() % gaps.size();
                    fireball.spawn(gaps[index], windowwidth);
                    fireballTimer.restart();
                }
            }

            // Player movement input
            if (Keyboard::isKeyPressed(Keyboard::Key::Right))
                a += movespeed;
            if (Keyboard::isKeyPressed(Keyboard::Key::Left))
                a -= movespeed;

            // --- Jetpack Effect Modification Start ---
            if (isJetpacked)
            {
                db = jetpackBoostSpeed; // Override gravity with jetpack boost
            }
            else
            {
                db += gravity; // Normal gravity
            }
            // --- Jetpack Effect Modification End ---

            // vertical movement

            b += db;

            // Game over if player falls below window
            if (b - playerHeight / 2.f > window.getSize().y)
            {
                isGameOver = true;
                continue;
            }
            score = static_cast<int>(worldHeight / 50);
            scoreText.setString("Score: " + to_string(score));

            if (score > highscore)
            {
                highscore = score;
                highscoreText.setString("High Score: " + to_string(highscore));
            }

            // Scroll world upwards if player above threshold h
            if (b < h)
            {
                float delta = h - b;
                b = h;
                worldHeight += delta;

                for (size_t i = 0; i < platforms.size(); ++i)
                {
                    platforms[i]->position.y += delta;

                    if (platforms[i]->position.y > window.getSize().y)
                    {
                        float newX = static_cast<float>(rand() % (window.getSize().x - platformWidth));

                        float minGap = 60.f;                        // Minimum vertical distance between platforms
                        float maxY = windowheight;                  // Start with screen height
                        float platformspeed = hardMode ? 4.f : 2.f; // Speed for moving platforms

                        // Find the highest (topmost) platform
                        for (auto *plat : platforms)
                        {
                            if (plat->position.y < maxY)
                            {
                                maxY = plat->position.y;
                            }
                        }

                        // Generate a new Y above the topmost platform with some spacing
                        float newY = maxY - verticalSpacing; // will ensure new platforms are recycled with proper spacing

                        delete platforms[i];

                        int r = rand() % 100;
                        Platform *newPlat = nullptr;

                        if (r < 70)
                            newPlat = new Platform(newX, newY, false, PlatformType::Normal);
                        else if (r < 90)
                            newPlat = new Platform(newX, newY, true, PlatformType::Moving, platformspeed);
                        else if (r < 95)
                            newPlat = new ThornPlatform(newX, newY);
                        else
                            newPlat = new DisappearingPlatform(newX, newY);

                        platforms[i] = newPlat;
                    }
                }
                // --- Jetpack Scrolling Start ---
                if (jetpackItem.isActive)
                {
                    jetpackItem.position.y += delta;
                }
                // --- Jetpack Scrolling End ---
                if (fireball.isActive)
                {
                    fireball.position.y += delta;
                }
                if (shield.isActive)
                {
                    shield.position.y += delta;
                }
            }

            // --- Jetpack Spawning Logic Start ---
            // Spawn jetpack every 10-20 seconds if not active and player is not jetpacked
            if ((score / jetpackInterval) > (lastJetpackScore / jetpackInterval) &&
                !jetpackItem.isActive && !isJetpacked &&
                jetpackEffectCooldownTimer.getElapsedTime().asSeconds() > jetpackEffectCooldownDuration)
            {
                vector<int> validPlatformIndices;

                // Find platforms that are currently visible on screen (or near player)
                for (int i = 0; i < platformCount; ++i)
                {
                    float platY = platforms[i]->position.y;

                    // Only spawn on platforms that are within the visible game area above the bottom
                    if (platY < windowheight / 2.0f && platY > 0.f)
                    { // Ensure it's not too high up or off screen
                        validPlatformIndices.push_back(i);
                    }
                }

                if (!validPlatformIndices.empty())
                {
                    int randomIndex = validPlatformIndices[rand() % validPlatformIndices.size()];
                    float x = platforms[randomIndex]->position.x + (platformWidth - (jetpackItem.texture.getSize().x)) / 2.f;
                    float y = platforms[randomIndex]->position.y - (jetpackItem.texture.getSize().y) - 5.f; // Slightly above platform
                    jetpackItem.spawn(x, y);
                    lastJetpackScore = (score / jetpackInterval) * jetpackInterval;
                }
            }
            // --- Jetpack Spawning Logic End ---

            // --- Jetpack Duration Logic Start ---
            // Turn off jetpack after 5 seconds of active use
            if (isJetpacked && jetpackActiveTimer.getElapsedTime().asSeconds() > 5.0f)
            {
                isJetpacked = false;
                jetpackEffectCooldownTimer.restart();
            }

            // Update fireball
            float fireballspeed = hardMode ? 9.f : 6.f; // Adjust speed based on difficulty
            fireball.update(windowwidth, fireballspeed);

            // Calculate player bounds
            float playerLeft = a - playerWidth / 2.f;
            float playerRight = a + playerWidth / 2.f;
            float playerTop = b - playerHeight / 2.f;
            float playerBottom = b + playerHeight / 2.f;

            // Spawn shield powerup between 5 and 15 seconds, if none active or shielded
            if ((score / shieldInterval) > (lastShieldScore / shieldInterval) &&
                !shield.isActive && !isShielded)
            {
                vector<int> validPlatformIndices;

                for (int i = 0; i < platforms.size(); ++i)
                {
                    Platform *plat = platforms[i];
                    if (!plat->isVisible() || plat->type == PlatformType::Thorn)
                        continue;

                    float platY = plat->position.y;
                    if (platY <= h && platY > 0.f)
                    {
                        validPlatformIndices.push_back(i);
                    }
                }

                if (!validPlatformIndices.empty())
                {
                    int randomIndex = validPlatformIndices[rand() % validPlatformIndices.size()];
                    float x = platforms[randomIndex]->position.x + (platformWidth - shieldWidth) / 2.f;
                    float y = platforms[randomIndex]->position.y - shieldHeight; // just above platform
                    shield.spawn(x, y);

                    lastShieldScore = (score / shieldInterval) * shieldInterval;
                }
            }

            // Shield powerup lasts 20 seconds when active
            if (isShielded && shieldActiveTimer.getElapsedTime().asSeconds() > 20.f)
            {
                isShielded = false;
            }

            // Fireball hits shield aura - cancels both
            if (fireball.isActive && isShielded)
            {
                FloatRect fireballBounds = fireball.getBounds(fireballWidth, fireballHeight);
                FloatRect auraBounds = shieldAura.getGlobalBounds();

                if (fireballBounds.findIntersection(auraBounds))
                {
                    fireball.isActive = false;
                    fireball.position = {-100.f, -100.f};
                    isShielded = false;
                }
            }

            // Check collision with fireball and player
            if (fireball.isActive && fireball.getBounds(fireballWidth, fireballHeight).findIntersection(player.getGlobalBounds()))
            {
                float intersectionLeft = std::max(playerLeft, fireball.position.x);
                float intersectionTop = std::max(playerTop, fireball.position.y);
                float intersectionRight = std::min(playerRight, fireball.position.x + fireballWidth);
                float intersectionBottom = std::min(playerBottom, fireball.position.y + fireballHeight);

                float intersectionWidth = intersectionRight - intersectionLeft;
                float intersectionHeight = intersectionBottom - intersectionTop;

                fireballSprite.setPosition(fireball.position);

                if (!isShielded && PerfectPixelCollision(player, image2, fireballSprite, image4,
                                                         intersectionLeft, intersectionTop, intersectionWidth, intersectionHeight))
                {
                    isGameOver = true;
                    continue;
                }
            }
            // Collision with platforms
            // Collision with platforms
            if (db > 3.3f)
            {
                bool jumped = false;

                for (auto *plat : platforms)
                {
                    if (!plat->isVisible())
                        continue;

                    float platTop = plat->position.y;
                    float platLeft = plat->position.x;
                    float platRight = platLeft + platformWidth;

                    float playerBottom = b + playerHeight / 2.f;
                    float playerPrevBottom = b + playerHeight / 2.f - db; // previous frame's bottom
                    float playerLeft = a - playerWidth / 2.f;
                    float playerRight = a + playerWidth / 2.f;

                    bool landedOnTop = playerPrevBottom <= platTop &&
                                       playerBottom >= platTop &&
                                       playerRight > platLeft &&
                                       playerLeft < platRight;

                    if (landedOnTop)
                    {
                        // Platform type check
                        if (plat->type == PlatformType::Thorn)
                        {
                            isGameOver = true;
                            break;
                        }

                        if (plat->type == PlatformType::Disappearing)
                        {
                            if (DisappearingPlatform *dp = dynamic_cast<DisappearingPlatform *>(plat))
                                dp->onPlayerTouch();
                        }

                        // Adjust Y position exactly to top of platform
                        b = platTop - playerHeight / 2.f;

                        if (plat->type == PlatformType::Disappearing)
                        {
                            if (DisappearingPlatform *dp = dynamic_cast<DisappearingPlatform *>(plat))
                                dp->onPlayerJump();
                        }

                        db = -playerJumpSpeed;
                        jumped = true;
                        break;
                    }
                }
            }

            // Player wrapping from one side of the screen to the other
            if (a > windowwidth + playerWidth / 2.f)
            {                           // If player goes off right side
                a = -playerWidth / 2.f; // Appear on the left side
            }
            if (a < (-(playerWidth / 2.f)))
            {                                        // If player goes off left side
                a = windowwidth + playerWidth / 2.f; // Appear on the right side
            }

            player.setPosition({a, b});

            jp.setPosition(sf::Vector2f(
                player.getPosition().x,
                player.getPosition().y));

            // Player picks up shield
            if (shield.isActive && shield.getBounds().findIntersection(player.getGlobalBounds()))
            {
                shield.deactivate();
                isShielded = true;
                shieldActiveTimer.restart();
            }
            // Player picks up jetpack
            if (jetpackItem.isActive && jetpackItem.getBounds().findIntersection(player.getGlobalBounds()))
            {
                isJetpacked = true;
                jetpackItem.deactivate();
                jetpackActiveTimer.restart();
            }
        }

        // Drawing
        window.clear();
        window.draw(bg);

        // Draw all platforms using their specific sprite
        for (auto *plat : platforms)
        {
            if (!plat->isVisible())
                continue;

            switch (plat->type)
            {
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
        // --- Draw Jetpack Item Start ---
        if (jetpackItem.isActive)
        {
            jetpackItem.draw(window);
        }

        if (shield.isActive)
            shield.draw(window);

        if (fireball.isActive)
        {
            fireballSprite.setPosition(fireball.position);
            //  Flip the sprite based on direction
            if (fireball.moveRight)
            {
                fireballSprite.setScale({1.f, 1.f}); // Normal facing right
            }
            else
            {
                fireballSprite.setScale({-1.f, 1.f}); // Flip horizontally to face left
                fireballSprite.setOrigin({fireballSprite.getLocalBounds().size.x, 0.f});
            }

            window.draw(fireballSprite);
        }

        // Draw player with jetpack or normal player
        if (isJetpacked)
        {
            window.draw(jp);
        }
        else
        {
            window.draw(player);
        }
        if (!isGameOver)
        {
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
        }
        else
        {
            // Draw Game Over overlay
            RectangleShape dim(Vector2f(windowwidth, windowheight));
            dim.setFillColor(Color(0, 0, 0, 180)); // semi-transparent black
            window.draw(dim);
            window.draw(over);
            window.draw(again);

            // Show final score and high score
            scoreText.setPosition({windowwidth / 3.333333333f, windowheight / 2.f});
            highscoreText.setPosition({windowwidth / 3.333333333f, windowheight / 1.8666666667f});
            window.draw(scoreText);
            window.draw(highscoreText);
        }

        window.display();

        ofstream outputFile("highscore.txt");

        if (outputFile.is_open())
        {
            outputFile << highscore;
            outputFile.close();
        }
    }
}