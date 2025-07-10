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
    Texture plattformtexture,playertexture,bgtexture,fireballtexture;
    
    if(!plattformtexture.loadFromFile("plat.png",false,IntRect({10,10},{platformWidth,platformHeight}))||
       !playertexture.loadFromFile("player.png",false,IntRect({0,0},{playerWidth,playerHeight}))||
       !bgtexture.loadFromFile("bg.png",false,IntRect({0,0},{windowwidth,windowheight}))||
       !fireballtexture.loadFromFile("fireball.png",false,IntRect({0,0},{fireballWidth, fireballHeight})))
    {
        cout<<"error loading texture"<<endl;
        return -1;
    }
    const Image& image1 = plattformtexture.copyToImage();
    const Image& image2 = playertexture.copyToImage();
    const Image& image3 = bgtexture.copyToImage();
    const Image& image4 = fireballtexture.copyToImage();

    Sprite platform(plattformtexture),player(playertexture),bg(bgtexture),fireballsprite(fireballtexture);
          
   
    vector<Platform>platforms;


    for(int i=0;i<platformCount;i++)
    {
        float x = static_cast<float>(rand() % (windowwidth - platformWidth)); // Keep inside screen width 
        float y = i * (windowheight / platformCount); // evenly spaced
        bool isMoving = rand()%100<30; // 30% chance to be moving
        platforms.emplace_back(x, y, isMoving);
    
    }
    // Player
    player.setOrigin({playerWidth/2.0f,playerHeight/2.0f }); // Center the player sprite

    const float movespeed = 3.f;
    const float playerJumpSpeed = 10.f;
    const float gravity = 0.2f;
    float a, b, h = 200; // a = x, b = y
    float da = 0, db = 0;
    
    // Start player on middle platform 
    int midIndex = platformCount / 2;
    float platformX = platforms[midIndex].position.x;
    float platformY = platforms[midIndex].position.y;

    a = platformX + platformWidth / 2.f;         // horizontally centered
    b = platformY - playerHeight/2.f;                // standing on top
    
    Fireball fireball;
    Clock fireballTimer;
    
    Shield shield;
    shield.load(); // Load texture
    Clock shieldSpawnTimer;
    Clock shieldActiveTimer;
    Clock shieldLifetimeTimer; // New timer to auto-despawn shield
    bool isShielded = false;
    float shieldWidth = shield.texture.getSize().x* shield.scaleFactor; // Scaled width
    float shieldHeight = shield.texture.getSize().y* shield.scaleFactor; // Scaled height
    //draw shieldaura
    CircleShape shieldAura;
    shieldAura.setRadius(50.f); // Radius slightly larger than player
    shieldAura.setFillColor(Color(0, 0, 255, 100)); // Blue with transparency
    shieldAura.setOrigin({shieldAura.getRadius(), shieldAura.getRadius()});


    while (window.isOpen())
    {
        while(auto event=window.pollEvent())
        {
         if (event->is<Event::Closed>())
            {
                window.close();
            }
        }
   
        for (auto& plat : platforms)
         {
       plat.update(windowwidth, platformWidth);
        }
        //Firball
        if (!fireball.isActive && fireballTimer.getElapsedTime().asSeconds() > 3.f) {
        vector<float> gaps;
        // Find gaps between platforms
        for (int i = 1; i < platforms.size(); ++i) {
        float upper = platforms[i - 1].position.y + platformHeight;
        float lower = platforms[i].position.y;

        if (lower - upper > fireballHeight + 10.f) {
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
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) a += movespeed;
        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) a -= movespeed;

        // Gravity and fall
        db += gravity;
        b += db;

        // Game Over if player falls below screen
        if (b - playerHeight/2.f > window.getSize().y)
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
            if (shield.isActive)
            {
                shield.position.y += -db;
            }
        }

        // Fireball update
        fireball.update(windowwidth);

        // Player bounds
      float playerLeft   = a - playerWidth / 2.f;
      float playerRight  = a + playerWidth / 2.f;
      float playerTop    = b - playerHeight / 2.f;
      float playerBottom = b + playerHeight / 2.f;
       
    // Spawn shield every 5-15 seconds
   if (!shield.isActive && !isShielded && shieldSpawnTimer.getElapsedTime().asSeconds() > (5 + rand() % 10)) {
    // Collect valid platforms where y is between h and windowheight
    vector<int> validPlatformIndices;
    for (int i = 0; i < platformCount; ++i) {
        float platY = platforms[i].position.y;
        if (platY <= h && platY >0.f) {
            validPlatformIndices.push_back(i);
        }
    }

    if (!validPlatformIndices.empty()) {
        int randomIndex = validPlatformIndices[rand() % validPlatformIndices.size()];
        float x = platforms[randomIndex].position.x + (platformWidth - shieldWidth) / 2.f;
        float y = platforms[randomIndex].position.y - shieldHeight ;// Just above platform
        shield.spawn(x, y);

        shieldLifetimeTimer.restart(); // Start auto-despawn timer here
        shieldSpawnTimer.restart();    // Reset spawn timer
    }
}

    // turn off the shield after 20 seconds
     if (isShielded && shieldActiveTimer.getElapsedTime().asSeconds() > 20.f) {
       isShielded = false;
     }
     
    // Check collision with shield aura
    if (fireball.isActive && isShielded) {
    FloatRect fireballBounds = fireball.getBounds(fireballWidth, fireballHeight);
    FloatRect auraBounds = shieldAura.getGlobalBounds();

    if (fireballBounds.findIntersection(auraBounds)) {
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
        float intersectionLeft   = std::max(playerLeft, fireball.position.x);
        float intersectionTop    = std::max(playerTop, fireball.position.y);
        float intersectionRight  = std::min(playerRight, fireball.position.x + fireballWidth);
        float intersectionBottom = std::min(playerBottom, fireball.position.y + fireballHeight);

        float intersectionWidth  = intersectionRight - intersectionLeft;
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


    float platLeft   = platPos.x;
    float platRight  = platPos.x + platformWidth;
    float platTop    = platPos.y;
    float platBottom = platPos.y + platformHeight;

    if (playerRight > platLeft &&
        playerLeft < platRight &&
        playerBottom > platTop &&
        playerTop < platBottom &&
        db > 0) // Falling
    {
        float intersectionLeft   = std::max(playerLeft, platLeft);
        float intersectionTop    = std::max(playerTop, platTop);
        float intersectionRight  = std::min(playerRight, platRight);
        float intersectionBottom = std::min(playerBottom, platBottom);
        float intersectionWidth  = intersectionRight - intersectionLeft;
        float intersectionHeight = intersectionBottom - intersectionTop;

        platform.setPosition(platPos); // platform sprite must match this platform
        if (PerfectPixelCollision(player, image2, platform, image1,  intersectionLeft, intersectionTop, intersectionWidth, intersectionHeight))
        {
             b = platTop - playerHeight / 2.f;  // Set player exactly on top
             db = -playerJumpSpeed;             // Apply jump velocity
        }
    }
}
        // Update player position
        player.setPosition({a, b});
        // Check shield collision(if player picks up shield)
    if (shield.isActive && shield.getBounds().findIntersection(player.getGlobalBounds())) {
        shield.deactivate();                    // Despawn
        isShielded = true;                      // Activate shield
        shieldActiveTimer.restart();           // Start timer
       }
    // Despawn shield if ignored for too long
    if (shield.isActive && shieldLifetimeTimer.getElapsedTime().asSeconds() > 10.f) {
        shield.deactivate();
    }
        // Draw
        window.clear();
        window.draw(bg);
        // Draw platforms
        for (const auto& plat : platforms)
         {
        platform.setPosition(plat.position);
        window.draw(platform);
         }
         //draw shield 
         if(shield.isActive)
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
          if (isShielded) {
         shieldAura.setPosition(player.getPosition());
         window.draw(shieldAura);
        }
        window.display();
    }

    return 0;
}

