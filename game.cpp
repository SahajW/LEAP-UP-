//game.cpp with new fireball and moving platforms feature
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
    const int playerwidth = 30;
    const int playerHeight = 30;
    const int fireballHeight = 30;
    const int fireballWidth = 30;
    Texture plattformtexture,playertexture,bgtexture,fireballtexture;
    
    if(!plattformtexture.loadFromFile("plat.png",false,IntRect({10,10},{platformWidth,platformHeight}))||
       !playertexture.loadFromFile("player.png",false,IntRect({0,0},{playerwidth,playerHeight}))||
       !bgtexture.loadFromFile("bg.png",false,IntRect({0,0},{windowwidth,windowheight}))||
       !fireballtexture.loadFromFile("fireball.png",false,IntRect({0,0},{fireballWidth, fireballHeight})))
    {
        cout<<"error loading texture"<<endl;
        return -1;
    }
 

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
    player.setOrigin({15.f, 15.f}); // Center the player sprite

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
        if (!fireball.isActive && fireballTimer.getElapsedTime().asSeconds() > 5.f) {
        vector<float> gaps;

        for (int i = 1; i < platforms.size(); ++i) {
        float upper = platforms[i - 1].position.y + platformHeight;
        float lower = platforms[i].position.y;

        if (lower - upper > fireballHeight + 10.f) {
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
        }

        // Fireball update
        fireball.update(windowwidth);
        // Check collision with fireball
        if (fireball.isActive && fireball.getBounds(fireballWidth, fireballHeight).findIntersection(player.getGlobalBounds()))
         {
           cout << "Hit by fireball!" << endl;
           window.close();
           }

        // Collision with platforms
        for (int i = 0; i < platformCount; ++i)
        {
            
            Vector2f platPos = platforms[i].position;

            if ( a + playerwidth/2.f > platPos.x &&
                 a - playerwidth/2.f < platPos.x + platformWidth &&
                 b + playerHeight/2.f > platPos.y &&
                 b + playerHeight/2.f < platPos.y + platformHeight &&
                db > 0)
            {
                db = -playerJumpSpeed;
                b = platPos.y - playerHeight/2.f; // Jump back up
            }
        }

        player.setPosition({a, b});

        // Draw
        window.clear();
        window.draw(bg);
        for (const auto& plat : platforms)
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

    return 0;
}
