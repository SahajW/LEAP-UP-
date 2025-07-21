#include "shield.hpp"
#include "pixelcollision.hpp"
#include "floating.hpp"
#include "fireball.hpp"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <vector>
#include"jetpack.hpp"

using namespace sf;
using namespace std;

int main()
{
    srand(static_cast<unsigned>(time(0)));

     // Get the primary monitor's video mode
    VideoMode desktopMode = VideoMode::getDesktopMode();

    // Calculate window size as a percentage of screen size (e.g., 70% width, 60% height)
  
    const int windowwidth = static_cast<int>((desktopMode.size.x / 2.65f));
    const int windowheight =  static_cast<int>((desktopMode.size.y / 1.142857143f));
    RenderWindow window(VideoMode({static_cast<unsigned int>(windowwidth), static_cast<unsigned int>(windowheight)}), "Leap UP");
    window.setFramerateLimit(60);

    const int platformCount = 15;
    const int platformWidth = 60;
    const int platformHeight = 15;
    const int playerWidth = 58;
    const int playerHeight = 83;
    const int fireballHeight = 30;
    const int fireballWidth = 30;

    // Load platform textures separately
    Texture platformTexture,thornTexture,disappearingTexture,playerTexture,bgTexture,fireballTexture,jplayertexture;


    if (!platformTexture.loadFromFile("plat.png", false, IntRect({10, 10}, {platformWidth, platformHeight})) ||
        !thornTexture.loadFromFile("thorn.png", false, IntRect({0, 0}, {platformWidth, platformHeight})) ||
        !disappearingTexture.loadFromFile("disappear.png", false, IntRect({0, 0}, {platformWidth, platformHeight})) ||
        !playerTexture.loadFromFile("player.png", false, IntRect({0, 0}, {playerWidth, playerHeight})) ||
        !bgTexture.loadFromFile("bg.jpeg", false, IntRect({0, 0}, {windowwidth, windowheight})) ||
        !fireballTexture.loadFromFile("fireball.png", false, IntRect({0, 0}, {fireballWidth, fireballHeight}))||
        !jplayertexture.loadFromFile("jplayer.png", false, IntRect({0, 0}, {playerWidth, playerHeight})) )
    {
        cout << "Error loading one or more textures" << endl;
        return -1;
    }

    const Image& image1 = platformTexture.copyToImage();
    const Image& image2 = playerTexture.copyToImage();
    const Image& image3 = bgTexture.copyToImage();
    const Image& image4 = fireballTexture.copyToImage();

    Sprite normalPlatformSprite(platformTexture);
    Sprite thornSprite(thornTexture);
    Sprite disappearingSprite(disappearingTexture);

    Sprite player(playerTexture),bg(bgTexture),fireballSprite(fireballTexture),jp(jplayertexture);

    // jp.scale({ 0.2f, 0.2f }); // Scale down the player sprite to fit the game
    jp.setOrigin({playerWidth / 2.f, playerHeight / 2.f}); //setting its scale to center 

     //platform creation
      vector<Platform*> platforms;

   float verticalSpacing = windowheight / static_cast<float>(platformCount);
   float currentY = windowheight - verticalSpacing;

   for (int i = 0; i < platformCount; ++i) {
       float x = static_cast<float>(rand() % (windowwidth - platformWidth));
       float y = currentY;
   
       Platform* plat = nullptr;
   
       int r = rand() % 100;
       if (r < 70)
           plat = new Platform(x, y, false, PlatformType::Normal);
       else if (r < 90)
           plat = new Platform(x, y, true, PlatformType::Moving);
       else if (r < 95)
           plat = new ThornPlatform(x, y);
       else
           plat = new DisappearingPlatform(x, y);

       platforms.push_back(plat);
       currentY -= verticalSpacing;  // evenly go upward
   }

    player.setOrigin({playerWidth / 2.f, playerHeight / 2.f});

    const float movespeed = 5.f;
    const float playerJumpSpeed = 10.f;
    const float gravity = 0.2f;
    float a, b, h = 200; // a=x, b=y
    float da = 0, db = 0;

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

    Fireball fireball;
    Clock fireballTimer;

    Shield shield;
    shield.load();
    Clock shieldSpawnTimer;
    Clock shieldActiveTimer;
    Clock shieldLifetimeTimer;
    bool isShielded = false;
    float shieldWidth = shield.texture.getSize().x * shield.scaleFactor;
    float shieldHeight = shield.texture.getSize().y * shield.scaleFactor;

    CircleShape shieldAura;
    shieldAura.setRadius(50.f);
    shieldAura.setFillColor(Color(0, 0, 255, 100));
    shieldAura.setOrigin({shieldAura.getRadius(), shieldAura.getRadius()});

    while (window.isOpen())
    {
        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }

        // Update platforms
        for (auto* plat : platforms) {
            plat->update(windowwidth, platformWidth);
        }

        // Fireball spawn logic
        if (!fireball.isActive && fireballTimer.getElapsedTime().asSeconds() > 3.f) {
            vector<float> gaps;
            for (size_t i = 1; i < platforms.size(); ++i) {
                Platform* prevPlat = platforms[i - 1];
                Platform* currPlat = platforms[i];

                if (!prevPlat->isVisible() || !currPlat->isVisible())
                    continue;

                if (prevPlat->type == PlatformType::Thorn || currPlat->type == PlatformType::Thorn)
                    continue;

                float upper = prevPlat->position.y + platformHeight;
                float lower = currPlat->position.y;

                if (lower - upper > fireballHeight + 10.f) {
                    float centerY = upper + (lower - upper) / 2.f - fireballHeight / 2.f;
                    gaps.push_back(centerY);
                }
            }

            if (!gaps.empty()) {
                int index = rand() % gaps.size();
                fireball.spawn(gaps[index]);
                fireballTimer.restart();
            }
        }

        // Player movement input
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) a += movespeed;
        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) a -= movespeed;
       
          // --- Jetpack Effect Modification Start ---
        if (isJetpacked) {
            db = jetpackBoostSpeed; // Override gravity with jetpack boost
        }
        else {
            db += gravity; // Normal gravity
        }
        // --- Jetpack Effect Modification End ---

        // vertical movement
        
        b += db;

        // Game over if player falls below window
        if (b - playerHeight / 2.f > window.getSize().y) {
            cout << "Game Over!" << endl;
            window.close();
        }

        // Scroll world upwards if player above threshold h
        if (b < h) {
            float delta = h - b;
            b = h;

            for (size_t i = 0; i < platforms.size(); ++i) {
                platforms[i]->position.y += delta;

                if (platforms[i]->position.y > window.getSize().y) {
                   float newX = static_cast<float>(rand() % (window.getSize().x - platformWidth));

                   // 🆕 Calculate a good Y position above current platforms
                   float minGap = 60.f; // Minimum vertical distance between platforms
                   float maxY = windowheight; // Start with screen height

                   // Find the highest (topmost) platform
                    for (auto* plat : platforms) {
                        if (plat->position.y < maxY) {
                        maxY = plat->position.y;
                        }
                    }

                // Generate a new Y above the topmost platform with some spacing
                float newY = maxY - verticalSpacing; // will ensure new platforms are recycled with proper spacing

                delete platforms[i];

                int r = rand() % 100;
                Platform* newPlat = nullptr;

                if (r < 70)
                    newPlat = new Platform(newX, newY, false, PlatformType::Normal);
                else if (r < 90)
                    newPlat = new Platform(newX, newY, true, PlatformType::Moving);
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
                jetpackItem.position.y +=delta;
               }
              // --- Jetpack Scrolling End ---
            if (fireball.isActive) {
                fireball.position.y += delta;
            }
            if (shield.isActive) {
                shield.position.y += delta;
            }
        }
        
         // --- Jetpack Spawning Logic Start ---
        // Spawn jetpack every 10-20 seconds if not active and player is not jetpacked
        if (!jetpackItem.isActive && !isJetpacked &&
            jetpackSpawnTimer.getElapsedTime().asSeconds() > (10 + rand() % 5) &&           //"Increased time for jetpack spawnning"
            jetpackEffectCooldownTimer.getElapsedTime().asSeconds() > jetpackEffectCooldownDuration) {
            vector<int> validPlatformIndices;


            // Find platforms that are currently visible on screen (or near player)
            for (int i = 0; i < platformCount; ++i) {
                float platY = platforms[i]->position.y;

                // Only spawn on platforms that are within the visible game area above the bottom
                if (platY < windowheight / 2.0f && platY > 0.f) { // Ensure it's not too high up or off screen
                    validPlatformIndices.push_back(i);
                }

            }

            if (!validPlatformIndices.empty()) {
                int randomIndex = validPlatformIndices[rand() % validPlatformIndices.size()];
                float x = platforms[randomIndex]->position.x + (platformWidth - (jetpackItem.texture.getSize().x * jetpackItem.scaleFactor)) / 2.f;
                float y = platforms[randomIndex]->position.y - (jetpackItem.texture.getSize().y * jetpackItem.scaleFactor) - 5.f; // Slightly above platform
                jetpackItem.spawn(x, y);

                jetpackLifetimeTimer.restart(); // Start auto-despawn timer here
                jetpackSpawnTimer.restart();    // Reset spawn timer
            }
        }
        // --- Jetpack Spawning Logic End ---
        
        // --- Jetpack Duration Logic Start ---
        // Turn off jetpack after 5 seconds of active use
        if (isJetpacked && jetpackActiveTimer.getElapsedTime().asSeconds() > 5.0f) {
            isJetpacked = false;
            jetpackEffectCooldownTimer.restart(); // --- NEW: Start cooldown timer ---
        }
        // --- Jetpack Duration Logic End ---

        // --- Jetpack Auto-Despawn if not picked up Start ---
        if (jetpackItem.isActive && jetpackLifetimeTimer.getElapsedTime().asSeconds() > 10.0f) {
            jetpackItem.deactivate();
        }
        // --- Jetpack Auto-Despawn if not picked up End ---

        // Update fireball
        fireball.update(windowwidth);

        // Calculate player bounds
        float playerLeft = a - playerWidth / 2.f;
        float playerRight = a + playerWidth / 2.f;
        float playerTop = b - playerHeight / 2.f;
        float playerBottom = b + playerHeight / 2.f;

        // Spawn shield powerup between 5 and 15 seconds, if none active or shielded
        if (!shield.isActive && !isShielded && shieldSpawnTimer.getElapsedTime().asSeconds() > (5 + rand() % 10)) {
            vector<int> validPlatformIndices;

            for (int i = 0; i < platforms.size(); ++i) {
                Platform* plat = platforms[i];
                if (!plat->isVisible() || plat->type == PlatformType::Thorn)
                    continue;

                float platY = plat->position.y;
                if (platY <= h && platY > 0.f) {
                    validPlatformIndices.push_back(i);
                }
            }

            if (!validPlatformIndices.empty()) {
                int randomIndex = validPlatformIndices[rand() % validPlatformIndices.size()];
                float x = platforms[randomIndex]->position.x + (platformWidth - shieldWidth) / 2.f;
                float y = platforms[randomIndex]->position.y - shieldHeight; // just above platform
                shield.spawn(x, y);

                shieldLifetimeTimer.restart();
                shieldSpawnTimer.restart();
            }
        }

        // Shield powerup lasts 20 seconds when active
        if (isShielded && shieldActiveTimer.getElapsedTime().asSeconds() > 20.f) {
            isShielded = false;
        }

        // Fireball hits shield aura - cancels both
        if (fireball.isActive && isShielded) {
            FloatRect fireballBounds = fireball.getBounds(fireballWidth, fireballHeight);
            FloatRect auraBounds = shieldAura.getGlobalBounds();

            if (fireballBounds.findIntersection(auraBounds)) {
                fireball.isActive = false;
                fireball.position = {-100.f, -100.f};
                isShielded = false;
            }
        }

        // Check collision with fireball and player
        if (fireball.isActive && fireball.getBounds(fireballWidth, fireballHeight).findIntersection(player.getGlobalBounds())) {
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
for (auto* plat : platforms) {
    if (!plat->isVisible()) continue;

    Vector2f platPos = plat->position;

    float platLeft = platPos.x;
    float platRight = platPos.x + platformWidth;
    float platTop = platPos.y;
    float platBottom = platPos.y + platformHeight;

    if (playerRight > platLeft && playerLeft < platRight &&
        playerBottom > platTop && playerTop < platBottom && db > 0)
    {
        if (plat->type == PlatformType::Thorn) {
            cout << "Game Over! Stepped on thorns." << endl;
            window.close();
            break;
        }

        // --- Touch Detected (before collision check) ---
        if (plat->type == PlatformType::Disappearing) {
            DisappearingPlatform* dp = dynamic_cast<DisappearingPlatform*>(plat);
            if (dp) {
                dp->onPlayerTouch();  // ✅ Record that player has landed
            }
        }

        // Compute intersection area for pixel-perfect check
        float intersectionLeft = std::max(playerLeft, platLeft);
        float intersectionTop = std::max(playerTop, platTop);
        float intersectionRight = std::min(playerRight, platRight);
        float intersectionBottom = std::min(playerBottom, platBottom);
        float intersectionWidth = intersectionRight - intersectionLeft;
        float intersectionHeight = intersectionBottom - intersectionTop;

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

            // --- Jump after Touch Detected ---
            if (plat->type == PlatformType::Disappearing) {
                DisappearingPlatform* dp = dynamic_cast<DisappearingPlatform*>(plat);
                if (dp) {
                    dp->onPlayerJump();  // ✅ Trigger disappearance after 1 jump
                }
            }
        }

        if (!isJetpacked && (plat->type != PlatformType::Disappearing)) {
             db = -10;
        }


    }
}
  
       // Player wrapping from one side of the screen to the other
        if (a > windowwidth + playerWidth / 2.f) { // If player goes off right side
            a = -playerWidth / 2.f; // Appear on the left side
        }
        if (a < (-(playerWidth / 2.f))) { // If player goes off left side
            a = windowwidth + playerWidth / 2.f; // Appear on the right side
        }

        player.setPosition({a, b});
        
        // --- NEW: Position jetpack flames relative to player ---
        // Adjust these offsets to fine-tune where the flames appear
        /*flame1.setPosition(player.getPosition().x - playerRadius * 0.3f, player.getPosition().y + playerRadius * 0.8f);*/
        jp.setPosition(sf::Vector2f(
            player.getPosition().x , 
            player.getPosition().y  
        ));

        // Player picks up shield
        if (shield.isActive && shield.getBounds().findIntersection(player.getGlobalBounds())) {
            shield.deactivate();
            isShielded = true;
            shieldActiveTimer.restart();
        }
         // Player picks up jetpack
        if (jetpackItem.isActive && jetpackItem.getBounds().findIntersection(player.getGlobalBounds())) {
            isJetpacked = true;
            jetpackItem.deactivate();
            jetpackActiveTimer.restart();
        }



        // Shield disappears if ignored too long
        if (shield.isActive && shieldLifetimeTimer.getElapsedTime().asSeconds() > 10.f) {
            shield.deactivate();
        }

        // Drawing
        window.clear();
        window.draw(bg);

        // Draw all platforms using their specific sprite
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
         // --- Draw Jetpack Item Start ---
        if (jetpackItem.isActive) {
            jetpackItem.draw(window);
        }

        if (shield.isActive)
            shield.draw(window);

        if (fireball.isActive) {
            fireballSprite.setPosition(fireball.position);
            window.draw(fireballSprite);
        }

        // Draw player with jetpack or normal player
        if (isJetpacked) {
            window.draw(jp);
        }
        else {
            window.draw(player);
        }

        if (isShielded) {
            shieldAura.setPosition(player.getPosition());
            window.draw(shieldAura);
        }

        window.display();
    }

    // Cleanup platforms
    for (auto* plat : platforms) {
        delete plat;
    }
    platforms.clear();

    return 0;
}