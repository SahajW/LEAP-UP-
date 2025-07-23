#include "shield.hpp"
#include "pixelcollision.hpp"
#include "floating.hpp"
#include "fireball.hpp"
#include "jetpack.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <ctime>
#include <vector>

using namespace sf;
using namespace std;
void runGame(RenderWindow &window)
{

    srand(static_cast<unsigned>(time(0)));

    window.setFramerateLimit(60);

    const int windowwidth = window.getSize().x;
    const int windowheight = window.getSize().y;

    const int platformCount = 15;
    const int platformWidth = 128;
    const int platformHeight = 21;
    const int playerWidth = 58;
    const int playerHeight = 83;
    const int fireballHeight = 30;
    const int fireballWidth = 30;
    const float gameWidth = 1000.f;
    const float gameHeight = 1400.f;
    int score = 0;
    float worldHeight = 0.f;

    int lastShieldScore = -1000;
    int lastJetpackScore = -1000;
    const int shieldInterval = 50;   // spawn every 300 score
    const int jetpackInterval = 200; // spawn every 500 score

    int highscore = 0;
    ifstream inputFile("highscore.txt");
    if (inputFile.is_open())
    {
        inputFile >> highscore;
        inputFile.close();
    }

    sf::Font font;
    !font.openFromFile("pixel.ttf");

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

    // Load platform textures separately
    Texture platformTexture, thornTexture, disappearingTexture, playerright, playerleft, bgTexture, fireballTexture, jplayertexture,
        pauseTexture, resumeTexture,
        QuitTexture, resumeTexture2, QuitTexture2;

    if (!platformTexture.loadFromFile("plat1.png", false, IntRect({0, 0}, {platformWidth, platformHeight})) ||
        !thornTexture.loadFromFile("plat3.png", false, IntRect({0, 0}, {platformWidth, platformHeight})) ||
        !disappearingTexture.loadFromFile("plat2.png", false, IntRect({0, 0}, {platformWidth, platformHeight})) ||
        !playerright.loadFromFile("player.png", false, IntRect({0, 0}, {playerWidth, playerHeight})) ||
        !playerleft.loadFromFile("player2.png", false, IntRect({0, 0}, {playerWidth, playerHeight})) ||
        !bgTexture.loadFromFile("bg1.png", false, IntRect({0, 0}, {windowwidth, windowheight})) ||
        !fireballTexture.loadFromFile("fireball.png", false, IntRect({0, 0}, {fireballWidth, fireballHeight})) ||
        !jplayertexture.loadFromFile("jplayer.png", false, IntRect({0, 0}, {playerWidth, playerHeight})) ||
        !pauseTexture.loadFromFile("pause.png") ||
        !resumeTexture.loadFromFile("resume.png") ||
        !QuitTexture.loadFromFile("quit.png") ||
        !resumeTexture2.loadFromFile("resume2.png") ||
        !QuitTexture2.loadFromFile("exit2.png"))
    {
        cout << "Error loading one or more textures" << endl;
        return;
    }

    const Image &image1 = platformTexture.copyToImage();
    const Image &image2 = playerright.copyToImage();
    const Image &image3 = playerleft.copyToImage();
    const Image &image4 = bgTexture.copyToImage();
    const Image &image5 = fireballTexture.copyToImage();

    Sprite normalPlatformSprite(platformTexture);
    Sprite thornSprite(thornTexture);
    Sprite disappearingSprite(disappearingTexture);

    Sprite player(playerright), bg(bgTexture), fireballSprite(fireballTexture), jp(jplayertexture),
        pause(pauseTexture), resume(resumeTexture), quit(QuitTexture), resume2(resumeTexture2), quit2(QuitTexture2);
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
            plat = new Platform(x, y, true, PlatformType::Moving, static_cast<float>(rand() % 3 + 2)); // speed between 2 and 4
        else if (r < 95)
            plat = new ThornPlatform(x, y);
        else
            plat = new DisappearingPlatform(x, y);

        platforms.push_back(plat);
        currentY -= verticalSpacing; // evenly go upward
    }

    player.setOrigin({playerWidth / 2.f, playerHeight / 2.f});

    const float movespeed = 5.f;
    const float playerJumpSpeed = 10.f;
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

    //  Clock for jetpack spawn cooldown after effect wears off
    Clock jetpackEffectCooldownTimer;
    const float jetpackEffectCooldownDuration = 5.0f; // cooldown duration in seconds
    jetpackEffectCooldownTimer.restart();             // Will be reset when jetpack effect ends
    // --- Jetpack Integration End ---

    Fireball fireball;
    Clock fireballTimer;

    Shield shield;
    shield.load();
    Clock shieldActiveTimer;
    bool isShielded = false;
    float shieldWidth = shield.texture.getSize().x * shield.scaleFactor;
    float shieldHeight = shield.texture.getSize().y * shield.scaleFactor;

    CircleShape shieldAura;
    shieldAura.setRadius(50.f);
    shieldAura.setFillColor(Color(0, 0, 255, 100));
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
            score = static_cast<int>(worldHeight / 50);        // <-- SCORE RELATED
            scoreText.setString("Score: " + to_string(score)); // <-- SCORE RELATED
            bool hardMode = (score >= 1000);                   // Hard mode starts at score 1000

            float gravity = hardMode ? 0.25f : 0.2f;
            float playerJumpSpeed = hardMode ? 12.f : 10.f;

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
            if (!fireball.isActive && fireballTimer.getElapsedTime().asSeconds() > 2.5f)
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

                        //  Only add the gap if it's ABOVE the player
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

                    //  Spawn the fireball at a valid Y, with random direction support
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
                cout << "Game Over!" << endl;
                window.close();
            }
            

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

                        //  Calculate a good Y position above current platforms
                        float minGap = 60.f;       // Minimum vertical distance between platforms
                        float maxY = windowheight; // Start with screen height
                        
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

                        if (!hardMode)
                        {
                            if (r < 70)
                                newPlat = new Platform(newX, newY, false, PlatformType::Normal);
                            else if (r < 90)
                                newPlat = new Platform(newX, newY, true, PlatformType::Moving, platformspeed);
                            else if (r < 95)
                                newPlat = new DisappearingPlatform(newX, newY);
                            else
                                newPlat = new ThornPlatform(newX, newY);
                        }
                        else
                        {
                            if (r < 65)
                                newPlat = new Platform(newX, newY, false, PlatformType::Normal);
                            else if (r < 85)
                                newPlat = new Platform(newX, newY, true, PlatformType::Moving, platformspeed);
                            else if (r < 95)
                                newPlat = new DisappearingPlatform(newX, newY);
                            else
                                newPlat = new ThornPlatform(newX, newY);
                        }
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
            // Spawn jetpack when score increases by jetpackInterval and player isn't already jetpacked
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
                    float x = platforms[randomIndex]->position.x + (platformWidth - (jetpackItem.texture.getSize().x * jetpackItem.scaleFactor)) / 2.f;
                    float y = platforms[randomIndex]->position.y - (jetpackItem.texture.getSize().y * jetpackItem.scaleFactor) - 5.f; // Slightly above platform
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
                jetpackEffectCooldownTimer.restart(); //  Start cooldown timer
            }
            // Update fireball
            float fireballspeed = hardMode ? 9.f : 6.f; // Adjust speed based on difficulty
            fireball.update(windowwidth,fireballspeed);

            // Calculate player bounds
            float playerLeft = a - playerWidth / 2.f;
            float playerRight = a + playerWidth / 2.f;
            float playerTop = b - playerHeight / 2.f;
            float playerBottom = b + playerHeight / 2.f;

            // Spawn shield when score increases by interval
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
                    if (platY <= h && platY > 0.f) // On screen
                    {
                        validPlatformIndices.push_back(i);
                    }
                }

                if (!validPlatformIndices.empty())
                {
                    int randomIndex = validPlatformIndices[rand() % validPlatformIndices.size()];
                    float x = platforms[randomIndex]->position.x + (platformWidth - shieldWidth) / 2.f;
                    float y = platforms[randomIndex]->position.y - shieldHeight;
                    shield.spawn(x, y);
                    lastShieldScore = (score / shieldInterval) * shieldInterval; // ✅ update spawn tracking
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
                    cout << "Hit by fireball!" << endl;
                    window.close();
                }
            }

            // Collision with platforms
            // Only attempt platform collision if the player is falling fast enough
            if (db > 3.25f)
            {
                bool jumped = false; // Track whether a valid jump (collision) happened

                for (auto *plat : platforms) // Loop through all platforms
                {
                    if (!plat->isVisible()) // Skip invisible platforms
                        continue;

                    Vector2f platPos = plat->position; // Get platform's position

                    // Calculate platform's bounding box
                    float platLeft = platPos.x;
                    float platRight = platPos.x + platformWidth;
                    float platTop = platPos.y;
                    float platBottom = platPos.y + platformHeight;

                    // Check if the player's bounding box intersects the platform's bounding box
                    if (playerRight > platLeft && playerLeft < platRight &&
                        playerBottom > platTop && playerTop < platBottom)
                    {
                        // If it's a thorn platform, trigger game over
                        if (plat->type == PlatformType::Thorn)
                        {
                            cout << "Game Over! Stepped on thorns." << endl;
                            window.close();
                            break;
                        }

                        // If it's a disappearing platform, trigger "touched" logic
                        if (plat->type == PlatformType::Disappearing)
                        {
                            if (DisappearingPlatform *dp = dynamic_cast<DisappearingPlatform *>(plat))
                            {
                                dp->onPlayerTouch(); // Mark that player landed on it
                            }
                        }

                        // Calculate the overlapping rectangle (intersection) between player and platform
                        float intersectionLeft = std::max(playerLeft, platLeft);
                        float intersectionTop = std::max(playerTop, platTop);
                        float intersectionRight = std::min(playerRight, platRight);
                        float intersectionBottom = std::min(playerBottom, platBottom);
                        float intersectionWidth = intersectionRight - intersectionLeft;
                        float intersectionHeight = intersectionBottom - intersectionTop;

                        // Choose the correct sprite image for the platform
                        Sprite *collisionSprite = nullptr;
                        switch (plat->type)
                        {
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

                        collisionSprite->setPosition(platPos); // Set the sprite's position to platform's

                        // Perform pixel-perfect collision using overlap bounds
                        if (PerfectPixelCollision(player, image2, *collisionSprite, image1,
                                                  intersectionLeft, intersectionTop,
                                                  intersectionWidth, intersectionHeight))
                        {
                            // Align the player's feet to the top of the platform
                            b = platTop - playerHeight / 2.f;

                            jumped = true; // Mark that jump happened (we landed on something)

                            // If it's a disappearing platform, trigger the "jump" logic to disappear it
                            if (plat->type == PlatformType::Disappearing)
                            {
                                if (DisappearingPlatform *dp = dynamic_cast<DisappearingPlatform *>(plat))
                                {
                                    dp->onPlayerJump(); // Trigger disappearance
                                }
                            }

                            break; // Stop checking other platforms (only one jump at a time)
                        }
                    }
                }

                // Only apply jump force if pixel-perfect collision occurred, and we're not jetpacking
                if (jumped && !isJetpacked)
                {
                    db = -playerJumpSpeed; // Reverse velocity to make the player jump
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

            // --- NEW: Position jetpack flames relative to player ---
            // Adjust these offsets to fine-tune where the flames appear
            // flame1.setPosition(player.getPosition().x - playerRadius * 0.3f, player.getPosition().y + playerRadius * 0.8f);
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
            // draw fireball if active
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
    cout << "your score is" << score << endl;
}