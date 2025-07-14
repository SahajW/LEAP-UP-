#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <vector>

using namespace sf;
using namespace std;

int main()
{
    srand(static_cast<unsigned>(time(0)));

    const int windowwidth = 1000;
    const int windowheight = 1400;
    RenderWindow window(VideoMode({windowwidth, windowheight}), "Leap UP");
    window.setFramerateLimit(60);

    const int platformCount = 15;
    const int platformWidth = 128;
    const int platformHeight = 49;
    const int playerwidth = 47;
    const int playerHeight = 82;
    

    Texture plattformtexture,playertexture,bgtexture;
    
    
    if(!plattformtexture.loadFromFile("assets/plat.png",false,IntRect({0,0},{platformWidth,platformHeight}))||
       !playertexture.loadFromFile("assets/Idle2.png",false,IntRect({0,0},{playerwidth,playerHeight}))||
       !bgtexture.loadFromFile("assets/bg1.png",false,IntRect({0,0},{windowwidth,windowheight})))
    {
        cout<<"error loading texture"<<endl;
        return -1;
    }
 

    Sprite platform(plattformtexture),player(playertexture),bg(bgtexture);
    
    float scaleX = static_cast<float>(platformWidth) / plattformtexture.getSize().x;
    float scaleY = static_cast<float>(platformHeight) / plattformtexture.getSize().y;

    
   
    Vector2f(plat[platformCount]);


    for(int i=0;i<platformCount;i++)
    {
        plat[i].x = static_cast<float>(rand() %(windowwidth - static_cast<int>(platformWidth))); // Keep inside screen width (400 - 60 )
        plat[i].y = i * (windowheight / platformCount); // evenly spaced
        
    
    }
    // Player
    player.setOrigin({64.f, 64.f}); // Center the player sprite

    const float movespeed = 3.f;
    const float playerJumpSpeed = 10.f;
    const float gravity = 0.2f;
    float a, b, h = 300; // a = x, b = y
    float da = 0, db = 0;

    // Start player on middle platform 
    int midIndex = platformCount/2 ;
    float platformX = plat[midIndex].x;
    float platformY = plat[midIndex].y;

    a = platformX + (platformWidth) / 2.f;         // horizontally centered
    b = platformY - (playerHeight)/2.f;                // standing on top


    while (window.isOpen())
    {
        while(auto event=window.pollEvent())
        {
         if (event->is<Event::Closed>())
            {
                window.close();
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
                plat[i].y += -db;
                if (plat[i].y > window.getSize().y)
                {
                    plat[i].y = 0;
                    plat[i].x = rand() % (window.getSize().x - static_cast<int>(platformWidth));
                }
                
             
            }
        }

        // Collision with platforms
        for (int i = 0; i < platformCount; ++i)
        {
            Vector2f pPos = {a, b};
            Vector2f platPos = plat[i];

            if (pPos.x + playerwidth/2.f > platPos.x &&
                pPos.x - playerwidth/2.f < platPos.x + (platformWidth) &&
                pPos.y + playerHeight/2.f > platPos.y &&
                pPos.y + playerHeight/2.f < platPos.y + (platformHeight) &&
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
        for(int i=0; i < platformCount; ++i)
        {
            platform.setPosition(plat[i]);
            window.draw(platform);
        }
        window.draw(player);
        window.display();
    }

    return 0;
}