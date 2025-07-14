#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <vector>
#include "Jetpack2.h" // Assuming Jetpack2.h is the file containing the jetpack class

using namespace sf;
using namespace std;

int main()
{
    srand(static_cast<unsigned>(time(0)));

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    // Get the primary monitor's video mode
    VideoMode desktopMode = VideoMode::getDesktopMode();

    // Calculate window size as a percentage of screen size (e.g., 70% width, 60% height)
    const sf::Vector2u windowSize(
        static_cast<unsigned int>(round(desktopMode.size.x / 2.65f)),
        static_cast<unsigned int>(round(desktopMode.size.y / 1.142857143f))
    );
    ////////////////////////////////////////////////////////////////////////////////////////////////////


    RenderWindow window(VideoMode({ windowSize.x, windowSize.y }), "Leap UP", Style::Titlebar | Style::Close);//Disabled maximization of program
    window.setFramerateLimit(60);

    const int platformCount = 16;


    Texture bgtexture, plattformtexture;

    if (!bgtexture.loadFromFile("bg.jpg")) {
        cout << "Error loading background texture" << endl;
        return -1;
    }

    if (!plattformtexture.loadFromFile("plat.png")) {
        cout << "Error loading platform texture" << endl;
        return -1;
    }



    // --- IMPORTANT CHANGE: Get platform dimensions directly from the texture ---
    // This ensures the hitbox (RectangleShape) matches the visual texture size.
    const float platformWidth = static_cast<float>(plattformtexture.getSize().x) / 7.0f;
    const float platformHeight = static_cast<float>(plattformtexture.getSize().y) / 10.0f;
    // --- END IMPORTANT CHANGE ---



    // Create background sprite
    Sprite bg(bgtexture);


    // Platforms
    vector<RectangleShape> plat(platformCount);
    vector<Vector2f> opos(platformCount); // Store positions for scrolling

    for (int i = 0; i < platformCount; ++i)
    {
        plat[i].setSize(Vector2f(platformWidth, platformHeight));
        plat[i].setTexture(&plattformtexture);
        float x = static_cast<float>(rand() % (windowSize.x - static_cast<int>(platformWidth))); // Keep inside screen width (400 - 60)
        float y = static_cast<float>(i * (windowSize.y / platformCount)); // evenly spaced
        plat[i].setPosition({ x, y });
        opos[i] = { x, y };
    }

    // Player
    const float playerRadius = windowSize.y / 60.0f;
    CircleShape player(playerRadius);
    player.setFillColor(Color::Yellow);
    player.setOrigin({ playerRadius, playerRadius });

    float a, b; // a = x, b = y
    float h = windowSize.y / 4.0f;
    float da = 0, db = 0;

    // Start player on middle platform 
    int midIndex = platformCount / 2;
    float platformX = plat[midIndex].getPosition().x;
    float platformY = plat[midIndex].getPosition().y;

    a = platformX + platformWidth / 2.f;          // horizontally centered
    b = platformY - playerRadius;                  // standing on top


    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // --- Jetpack Integration Start ---
    jetpack jetpackItem;
    try {
        jetpackItem.load(); // Load jetpack texture
    }
    catch (const std::runtime_error& e) {
        cout << e.what() << endl;
        return -1;
    }

    Clock jetpackSpawnTimer;
    Clock jetpackActiveTimer;
    Clock jetpackLifetimeTimer; // New timer to auto-despawn jetpack
    bool isJetpacked = false;
    const float jetpackBoostSpeed = -8.0f; // Stronger upward velocity when jetpacked

    // New: Clock for jetpack spawn cooldown after effect wears off
    Clock jetpackEffectCooldownTimer;
    const float jetpackEffectCooldownDuration = 5.0f; // For example, 5 seconds cooldown
    // Initialize to a high value so it doesn't block initial spawn
    jetpackEffectCooldownTimer.restart(); // Will be reset when jetpack effect ends
    // --- Jetpack Integration End ---
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
        float moveSpeed = windowSize.x / 180.0f;
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) a += moveSpeed;
        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) a -= moveSpeed;
        

        /////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Gravity and fall
        // --- Jetpack Effect Modification Start ---
        if (isJetpacked) {
            db = jetpackBoostSpeed; // Override gravity with jetpack boost
        }
        else {
            db += 0.2f; // Normal gravity
        }
        // --- Jetpack Effect Modification End ---
		//////////////////////////////////////////////////////////////////////////////////////////////////////////
        b += db;

        // Game Over if player falls below screen
        if (b - playerRadius > window.getSize().y)
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
                opos[i].y += -db;
                if (opos[i].y > windowSize.y)
                {
                    opos[i].y = 0;
                    opos[i].x = static_cast<float>(rand() % (windowSize.x - static_cast<int>(platformWidth)));;
                }
                plat[i].setPosition(opos[i]);
            }
            // --- Jetpack Scrolling Start ---
            if (jetpackItem.isActive)
            {
                jetpackItem.position.y += -db;
            }
            // --- Jetpack Scrolling End ---
        }

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // --- Jetpack Spawning Logic Start ---
        // Spawn jetpack every 10-20 seconds if not active and player is not jetpacked
        if (!jetpackItem.isActive && !isJetpacked &&
            jetpackSpawnTimer.getElapsedTime().asSeconds() > (30 + rand() % 10) &&          //"Increased time for jetpack spawnning"
            jetpackEffectCooldownTimer.getElapsedTime().asSeconds() > jetpackEffectCooldownDuration) {
            vector<int> validPlatformIndices;


            // Find platforms that are currently visible on screen (or near player)
            for (int i = 0; i < platformCount; ++i) {
                float platY = plat[i].getPosition().y;
                
                // Only spawn on platforms that are within the visible game area above the bottom
                if (platY < windowSize.y / 2.0f && platY > 0.f) { // Ensure it's not too high up or off screen
                    validPlatformIndices.push_back(i);
                }

            }

            if (!validPlatformIndices.empty()) {
                int randomIndex = validPlatformIndices[rand() % validPlatformIndices.size()];
                float x = plat[randomIndex].getPosition().x + (platformWidth - (jetpackItem.texture.getSize().x * jetpackItem.scaleFactor)) / 2.f;
                float y = plat[randomIndex].getPosition().y - (jetpackItem.texture.getSize().y * jetpackItem.scaleFactor) - 5.f; // Slightly above platform
                jetpackItem.spawn(x, y);

                jetpackLifetimeTimer.restart(); // Start auto-despawn timer here
                jetpackSpawnTimer.restart();    // Reset spawn timer
            }
        }
        // --- Jetpack Spawning Logic End ---
		//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // --- Jetpack Duration Logic Start ---
        // Turn off jetpack after 5 seconds of active use
        if (isJetpacked && jetpackActiveTimer.getElapsedTime().asSeconds() > 5.0f) {
            isJetpacked = false;
        }
        // --- Jetpack Duration Logic End ---

        // --- Jetpack Auto-Despawn if not picked up Start ---
        if (jetpackItem.isActive && jetpackLifetimeTimer.getElapsedTime().asSeconds() > 10.0f) {
            jetpackItem.deactivate();
        }
        // --- Jetpack Auto-Despawn if not picked up End ---
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        for (int i = 0; i < platformCount; ++i)
        {
            // Check for collision only if player is falling (db > 0)
            // and player's bottom edge is within platform's vertical range
            // and player's horizontal range overlaps with platform
            if (a + playerRadius > plat[i].getPosition().x && // Player's right edge past platform's left edge
                a - playerRadius < plat[i].getPosition().x + platformWidth && // Player's left edge past platform's right edge
                b + playerRadius > plat[i].getPosition().y && // Player's bottom edge below platform's top edge
                b + playerRadius < plat[i].getPosition().y + platformHeight && // Player's bottom edge above platform's bottom edge
                db > 0) // Player is falling
            {
                // --- Modified Jump Logic with Jetpack Check Start ---
                if (!isJetpacked) { // Only apply normal jump if not jetpacked
                    db = -10; // Apply jump velocity (negative db means moving upwards)
                }
                // Snap player's position to the top of the platform to prevent sinking
                b = plat[i].getPosition().y - playerRadius;
                // --- Modified Jump Logic with Jetpack Check End ---
            }
        }

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // --- Jetpack Collision with Player Start ---
        if (jetpackItem.isActive && jetpackItem.getBounds().findIntersection(player.getGlobalBounds())) {
            jetpackItem.deactivate();                    // Despawn
            isJetpacked = true;                      // Activate shield
            jetpackActiveTimer.restart();           // Start timer
        }
        // --- Jetpack Collision with Player End ---
        //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // Player wrapping from one side of the screen to the other
        if (a > windowSize.x + playerRadius) { // If player goes off right side
            a = -playerRadius; // Appear on the left side
        }
        if (a < (-playerRadius)) { // If player goes off left side
            a = windowSize.x + playerRadius; // Appear on the right side
        }
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



        player.setPosition({ a, b });

        // Draw
        window.clear();
        window.draw(bg);
        window.draw(player);
        for (const auto& p : plat)
        {
            window.draw(p);
        }
        // --- Draw Jetpack Start ---
        if (jetpackItem.isActive) {
            jetpackItem.draw(window);
        }
        // --- Draw Jetpack End ---

        window.display();
    }

    return 0;
}