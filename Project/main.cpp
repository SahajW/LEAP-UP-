#include <SFML/Graphics.hpp>
#include <iostream>     // For cout
#include <ctime>        // For time() in srand
#include <vector>       // For std::vector
#include <cstdlib>      // For rand(), srand()

#include "Platform.h" // Include our new Platform header

// Using declarations for convenience
using namespace sf;
using namespace std;

int main()
{
    srand(static_cast<unsigned>(time(0))); // Seed the random number generator

    /*
     * Window Creation
     */

    VideoMode screen = VideoMode::getDesktopMode();
    RenderWindow window(screen, "Leap up", Style::Default);
    window.setPosition({ 0, 0 });
    window.setFramerateLimit(60);

    /*
     * Background texture
     */
    Texture bgTexture;
    if (!bgTexture.loadFromFile("sprite/bg1.png"))
    {
        cout << "Failed to load background texture!" << endl;
        return -1;
    }
    Sprite backgroundSprite(bgTexture);

    const float gameWidth = 1000.f;
    const float gameHeight = 1400.f;

    View gameView(Vector2f(gameWidth / 2.f, gameHeight / 2.f), Vector2f(gameWidth, gameHeight));

    /*
     * Black bars on the sides (Letterboxing logic)
     */
    float windowRatio = static_cast<float>(screen.size.x) / screen.size.y;
    float gameRatio = gameWidth / gameHeight;
    float viewWidth = 1.f;
    float viewHeight = 1.f;
    float offsetX = 0.f;
    float offsetY = 0.f;

    if (windowRatio > gameRatio) {
        viewWidth = gameRatio / windowRatio;
        offsetX = (1.f - viewWidth) / 2.f;
    }
    else {
        viewHeight = windowRatio / gameRatio;
        offsetY = (1.f - viewHeight) / 2.f;
    }

    gameView.setViewport(FloatRect(Vector2f(offsetX, offsetY), Vector2f(viewWidth, viewHeight)));
    window.setView(gameView);

    /*
     * Game Objects - Platforms and Player
     */

    const int platformCount = 20;
    const float platformWidth = 170.f;
    const float platformHeight = 25.f;

    // Use a vector of our Platform objects
    vector<Platform> platforms;
    platforms.reserve(platformCount); // Pre-allocate memory for efficiency

    for (int i = 0; i < platformCount; ++i)
    {
        // Construct Platform objects and add them to the vector
        platforms.emplace_back(platformWidth, platformHeight, gameWidth, gameHeight, i, platformCount);
    }

    // Player
    const float playerRadius = 15.f;
    CircleShape player(playerRadius);
    player.setFillColor(Color::Yellow);
    player.setOrigin({ playerRadius, playerRadius });

    float playerX, playerY, scrollHeight = 200; // playerX = x, playerY = y
    float playerVelocityX = 0, playerVelocityY = 0;

    // Start player on a middle platform
    int midIndex = platformCount / 2;
    // Find a normal or moving platform to start on
    while ((platforms[midIndex].type == THORN || !platforms[midIndex].isSolid) && midIndex < platformCount - 1) {
        midIndex++; // Move to the next platform if it's a thorn or non-solid disappearing platform
    }
    if ((platforms[midIndex].type == THORN || !platforms[midIndex].isSolid)) { // If still problematic after searching, pick first valid
        for (int i = 0; i < platformCount; ++i) {
            if (platforms[i].type != THORN && platforms[i].isSolid) { // Checks if it's solid for starting position
                midIndex = i;
                break;
            }
        }
    }

    float initialPlatformX = platforms[midIndex].shape.getPosition().x;
    float initialPlatformY = platforms[midIndex].shape.getPosition().y;

    playerX = initialPlatformX + platformWidth / 2.f; // horizontally centered
    playerY = initialPlatformY - playerRadius;          // standing on top

    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }
        }

        // Movement input
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) playerX += 3;
        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) playerX -= 3;

        // Gravity and fall
        playerVelocityY += 0.2f; // Apply gravity
        playerY += playerVelocityY;

        // Game Over if player falls below screen
        if (playerY - playerRadius > gameHeight) // Use gameHeight for game world bounds
        {
            cout << "Game Over! You fell off the screen." << endl;
            window.close();
        }

        // Update all platforms using their update method
        for (int i = 0; i < platformCount; ++i)
        {
            platforms[i].update(gameWidth);
        }

        // Scroll world if player is above scrollHeight
        if (playerY < scrollHeight)
        {
            playerY = scrollHeight; // Keep player at scroll height
            for (int i = 0; i < platformCount; ++i)
            {
                platforms[i].scrollAndReset(playerVelocityY, gameWidth, gameHeight, platformWidth);
            }
        }

        // Collision with platforms
        Vector2f pPos = { playerX, playerY };

        for (int i = 0; i < platformCount; ++i)
        {
            Vector2f platPos = platforms[i].shape.getPosition();

            // Check for collision with the top of the platform when falling AND if the platform is solid
            if (platforms[i].isSolid && //Only collide if the platform is currently solid
                pPos.x + playerRadius > platPos.x &&
                pPos.x - playerRadius < platPos.x + platformWidth &&
                pPos.y + playerRadius > platPos.y &&
                pPos.y + playerRadius < platPos.y + platformHeight &&
                playerVelocityY > 0) // Only collide if player is falling
            {
                if (platforms[i].type == THORN)
                {
                    cout << "Game Over! You hit a thorn platform!" << endl;
                    window.close(); // End game if hit thorn platform
                }
                else // NORMAL, MOVING, or DISAPPEARING platform (when solid)
                {
                    playerVelocityY = -10; // Make player jump
                    playerY = platPos.y - playerRadius; // Snap player to top of platform

                    // If it's a disappearing platform, start its timer
                    if (platforms[i].type == DISAPPEARING) { // Start timer when player lands on it
                        platforms[i].disappearTimer = 15.f; // 15 frames = 0.25 second at 60 FPS
                    }
                }
            }
        }

        /*
         * Player right-left, left to right reappear
         */
        if (pPos.x > gameWidth + playerRadius) {
            playerX = -playerRadius; // Reappear on the left side
        }
        else if (pPos.x < -playerRadius) {
            playerX = gameWidth + playerRadius; // Reappear on the right side
        }
        player.setPosition({ playerX, playerY });

        window.clear(Color::Black);
        window.setView(gameView);

        window.draw(backgroundSprite); // Draw background sprite

        // Draw all platforms using their draw method
        for (int i = 0; i < platformCount; ++i)
            platforms[i].draw(window);
        window.draw(player);

        window.display();
    }
    return 0;
}
