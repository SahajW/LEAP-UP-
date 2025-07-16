//#include <SFML/Graphics.hpp>
//#include <iostream> // For cout
//#include <ctime>    // For time() in srand
//#include <vector>   // For std::vector
//#include <cstdlib>  // For rand(), srand()
//
//// Using declarations for convenience
//using namespace sf;
//using namespace std;
//
//// 1. Define an enum for platform types
//enum PlatformType {
//    NORMAL,
//    THORN,
//    MOVING,
//    DISAPPEARING
//};
//
//// 2. Defining a struct to hold all platform properties
//struct Platform {
//    RectangleShape shape;
//    Vector2f originalPosition; // Stores the base position for scrolling
//    PlatformType type;
//    float moveDirection;       // 1 for right, -1 for left
//    float moveSpeed;           // Speed of moving platforms
//
//    bool isSolid;              //True if player can stand on it.
//    float disappearTimer;
//};
//
//int main()
//{
//    srand(static_cast<unsigned>(time(0))); // Seed the random number generator
//
//    /*
//              Window Creation
//    */
//
//    VideoMode screen = VideoMode::getDesktopMode();
//    RenderWindow window(screen, "Leap up", Style::Default);
//    window.setPosition({ 0, 0 });
//    window.setFramerateLimit(60);
//
//    /*
//        Background texture
//    */
//    Texture bgTexture;
//    if (!bgTexture.loadFromFile("sprite/bg1.png"))
//    {
//        cout << "Failed to load background texture!" << endl;
//        return -1;
//    }
//    Sprite backgroundSprite(bgTexture);
//
//    const float gameWidth = 1000.f;
//    const float gameHeight = 1400.f;
//
//    View gameView(Vector2f(gameWidth / 2.f, gameHeight / 2.f), Vector2f(gameWidth, gameHeight));
//
//    /*
//              Black bars on the sides (Letterboxing logic)
//    */
//    float windowRatio = static_cast<float>(screen.size.x) / screen.size.y;
//    float gameRatio = gameWidth / gameHeight;
//    float viewWidth = 1.f;
//    float viewHeight = 1.f;
//    float offsetX = 0.f;
//    float offsetY = 0.f;
//
//    if (windowRatio > gameRatio) {
//        viewWidth = gameRatio / windowRatio;
//        offsetX = (1.f - viewWidth) / 2.f;
//    }
//    else {
//        viewHeight = windowRatio / gameRatio;
//        offsetY = (1.f - viewHeight) / 2.f;
//    }
//
//    gameView.setViewport(FloatRect(Vector2f(offsetX, offsetY), Vector2f(viewWidth, viewHeight)));
//    window.setView(gameView);
//
//    /*
//              Game Objects - Platforms and Player
//    */
//
//    const int platformCount = 20;
//    const float platformWidth = 170.f;
//    const float platformHeight = 25.f;
//
//    // 4. Use a vector of our new Platform struct
//    vector<Platform> platforms(platformCount);
//
//    for (int i = 0; i < platformCount; ++i)
//    {
//        platforms[i].shape.setSize(Vector2f(platformWidth, platformHeight));
//
//        int typeRoll = rand() % 100; // 0-99
//        if (typeRoll < 70) { // 75% chance for normal platforms
//            platforms[i].type = NORMAL;
//            platforms[i].shape.setFillColor(Color::Green); // Fallback color
//            platforms[i].isSolid = true; // Normal platforms are always solid
//        }
//        else if (typeRoll < 75) { // 5% chance for thorn platforms
//            platforms[i].type = THORN;
//            platforms[i].shape.setFillColor(Color::Red); // Fallback color
//            platforms[i].isSolid = true; // Thorn platforms are always solid
//        }
//        else if (typeRoll < 90) { // 10% chance for moving platforms
//            platforms[i].type = MOVING;
//            platforms[i].shape.setFillColor(Color::Blue); // Fallback color
//            platforms[i].moveDirection = (rand() % 2 == 0) ? 1.f : -1.f; // Random initial direction (right or left)
//            platforms[i].moveSpeed = (rand() % 2 + 1) * 1.5f; // Speed between 1.5 and 3.0
//            platforms[i].isSolid = true; // Moving platforms are always solid
//        }
//        else { // 10% chance for disappearing platforms
//            platforms[i].type = DISAPPEARING; // Type assignment
//            platforms[i].shape.setFillColor(Color(128, 0, 128)); // Purple color for disappearing
//            platforms[i].isSolid = true; // Starts solid
//            platforms[i].disappearTimer = 0.f; //Timer not active initially
//        }
//
//
//
//        // With this corrected line:
//        platforms[i].shape.move(Vector2f(platforms[i].moveDirection * platforms[i].moveSpeed, 0));
//        // Set initial position
//        float x = rand() % (static_cast<int>(gameWidth - platformWidth)); // Ensure platform stays within gameWidth
//        float y = i * (gameHeight / platformCount); // Evenly spaced vertically
//        platforms[i].shape.setPosition({ x, y });
//        platforms[i].originalPosition = { x, y }; // Store original position for scrolling
//    }
//
//    // Player
//    const float playerRadius = 15.f;
//    CircleShape player(playerRadius);
//    player.setFillColor(Color::Yellow);
//    player.setOrigin({ playerRadius, playerRadius });
//
//    float playerX, playerY, scrollHeight = 200; // playerX = x, playerY = y
//    float playerVelocityX = 0, playerVelocityY = 0;
//
//    // Start player on a middle platform
//    int midIndex = platformCount / 2;
//    // Find a normal or moving platform to start on
//    while ((platforms[midIndex].type == THORN || !platforms[midIndex].isSolid) && midIndex < platformCount - 1) {
//        midIndex++; // Move to the next platform if it's a thorn or non-solid disappearing platform
//    }
//    if ((platforms[midIndex].type == THORN || !platforms[midIndex].isSolid)) { // If still problematic after searching, pick first valid
//        for (int i = 0; i < platformCount; ++i) {
//            if (platforms[i].type != THORN && platforms[i].isSolid) { // Checks if it's solid for starting position
//                midIndex = i;
//                break;
//            }
//        }
//    }
//
//
//    float initialPlatformX = platforms[midIndex].shape.getPosition().x;
//    float initialPlatformY = platforms[midIndex].shape.getPosition().y;
//
//    playerX = initialPlatformX + platformWidth / 2.f; // horizontally centered
//    playerY = initialPlatformY - playerRadius;         // standing on top
//
//    while (window.isOpen())
//    {
//        while (auto event = window.pollEvent())
//        {
//            if (event->is<Event::Closed>())
//            {
//                window.close();
//            }
//        }
//
//        // Movement input
//        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) playerX += 3;
//        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) playerX -= 3;
//
//        // Gravity and fall
//        playerVelocityY += 0.2f; // Apply gravity
//        playerY += playerVelocityY;
//
//        // Game Over if player falls below screen
//        if (playerY - playerRadius > gameHeight) // Use gameHeight for game world bounds
//        {
//            cout << "Game Over! You fell off the screen." << endl;
//            window.close();
//        }
//
//        // 5. Update moving platform positions
//        for (int i = 0; i < platformCount; ++i)
//        {
//            if (platforms[i].type == MOVING)
//            {
//                // Move the platform horizontally
//                platforms[i].shape.move(Vector2f(platforms[i].moveDirection * platforms[i].moveSpeed, 0));
//
//                // Reverse direction if hitting game world boundaries
//                if (platforms[i].shape.getPosition().x <= 0 ||
//                    platforms[i].shape.getPosition().x + platformWidth >= gameWidth)
//                {
//                    platforms[i].moveDirection *= -1.f; // Reverse direction
//                }
//            }
//            else if (platforms[i].type == DISAPPEARING) // 
//            {
//                if (platforms[i].disappearTimer > 0) // If timer is active
//                {
//                    platforms[i].disappearTimer -= 30.f; // Decrement timer
//                    if (platforms[i].disappearTimer <= 0) // If timer runs out
//                    {
//                        platforms[i].isSolid = false; // Make it non-solid
//                        platforms[i].shape.setFillColor(Color::Transparent); //Make it invisible
//                    }
//                }
//            }
//        }
//
//        // Scroll world if player is above scrollHeight
//        if (playerY < scrollHeight)
//        {
//            playerY = scrollHeight; // Keep player at scroll height
//            for (int i = 0; i < platformCount; ++i)
//            {
//                // Adjust original position for scrolling
//                platforms[i].originalPosition.y += -playerVelocityY;
//
//                // If platform goes off screen bottom, reset to top with new properties
//                if (platforms[i].originalPosition.y > gameHeight)
//                {
//                    platforms[i].originalPosition.y = 0; // Reset to top
//                    platforms[i].originalPosition.x = rand() % (static_cast<int>(gameWidth - platformWidth)); // New random X
//
//                    // Re-randomize type for new platform appearing at the top
//                    int typeRoll = rand() % 100;
//                    if (typeRoll < 60) {
//                        platforms[i].type = NORMAL;
//                        platforms[i].shape.setFillColor(Color::Green);
//                        platforms[i].isSolid = true;
//                    }
//                    else if (typeRoll < 75) {
//                        platforms[i].type = THORN;
//                        platforms[i].shape.setFillColor(Color::Red);
//                        platforms[i].isSolid = true;
//                    }
//                    else if (typeRoll < 90) {
//                        platforms[i].type = MOVING;
//                        platforms[i].shape.setFillColor(Color::Blue);
//                        platforms[i].moveDirection = (rand() % 2 == 0) ? 1.f : -1.f;
//                        platforms[i].moveSpeed = (rand() % 2 + 1) * 1.5f;
//                        platforms[i].isSolid = true;
//                    }
//                    else { // For disappearing platforms when they reappear at the top
//                        platforms[i].type = DISAPPEARING;
//                        platforms[i].shape.setFillColor(Color(128, 0, 128)); //Reset to purple
//                        platforms[i].isSolid = true; // Reset to solid
//                        platforms[i].disappearTimer = 0.f; //Reset timer
//                    }
//                }
//                // Update the shape's position based on its original position (for scrolling)
//                // and its current horizontal offset if it's a moving platform.
//                // This ensures moving platforms maintain their horizontal movement relative to the scrolling world.
//                float currentOffsetX = platforms[i].shape.getPosition().x - platforms[i].originalPosition.x;
//                platforms[i].shape.setPosition(Vector2f(platforms[i].originalPosition.x + currentOffsetX, platforms[i].originalPosition.y));
//            }
//        }
//
        //// Collision with platforms
        //Vector2f pPos = { playerX, playerY };

        //for (int i = 0; i < platformCount; ++i)
        //{
        //    Vector2f platPos = platforms[i].shape.getPosition();

        //    // Check for collision with the top of the platform when falling AND if the platform is solid
        //    if (platforms[i].isSolid && //Only collide if the platform is currently solid
        //        pPos.x + playerRadius > platPos.x &&
        //        pPos.x - playerRadius < platPos.x + platformWidth &&
        //        pPos.y + playerRadius > platPos.y &&
        //        pPos.y + playerRadius < platPos.y + platformHeight &&
        //        playerVelocityY > 0) // Only collide if player is falling
        //    {
        //        if (platforms[i].type == THORN)
        //        {
        //            cout << "Game Over! You hit a thorn platform!" << endl;
        //            window.close(); // End game if hit thorn platform
        //        }
        //        else // NORMAL, MOVING, or DISAPPEARING platform (when solid)
        //        {
        //            playerVelocityY = -10; // Make player jump
        //            playerY = platPos.y - playerRadius; // Snap player to top of platform

        //            // If it's a disappearing platform, start its timer
        //            if (platforms[i].type == DISAPPEARING) { // Start timer when player lands on it
        //                platforms[i].disappearTimer = 60.f; // 60 frames = 1 second at 60 FPS
        //            }
        //        }
        //    }
        //}
//
//        /*
//              Player right-left, left to right reappear
//        */
//        if (pPos.x > gameWidth + playerRadius) {
//            playerX = -playerRadius; // Reappear on the left side
//        }
//        else if (pPos.x < -playerRadius) {
//            playerX = gameWidth + playerRadius; // Reappear on the right side
//        }
//        player.setPosition({ playerX, playerY });
//
//        window.clear(Color::Black);
//        window.setView(gameView);
//
//        window.draw(backgroundSprite); // Draw background sprite
//
//        // 6. Draw all platforms using their shape member
//        for (int i = 0; i < platformCount; ++i)
//            window.draw(platforms[i].shape);
//        window.draw(player);
//
//        window.display();
//    }
//    return 0;
//}
