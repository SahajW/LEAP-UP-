#include "game.hpp"
#include "manual.hpp"
#include "player.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include <vector>



enum class Scene { Menu, Game, Manual };

using namespace sf;
using namespace std;

void fadeIn(sf::RenderWindow& window, sf::Sprite& background,sf::Sprite& start,sf::Sprite& credits,sf::Sprite& quit,
    sf::Sprite& volume, sf::Music& music,sf::Sprite& manual,sf::Sprite& menu) {
    sf::RectangleShape overlay(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 255)); // Fully black

    const float fadeDuration = 1.0f; // seconds
    sf::Clock clock;

    music.play(); // Start music just before fade-in

    while (overlay.getFillColor().a > 0) {
        float elapsed = clock.getElapsedTime().asSeconds();
        float alpha = 255 - (elapsed / fadeDuration) * 255;
        if (alpha < 0) alpha = 0;

        overlay.setFillColor(sf::Color(0, 0, 0, static_cast<uint8_t>(alpha)));

        window.clear();
        window.draw(background);     // draw background underneath
        window.draw(start);          // draw start button
        window.draw(credits);        // draw credits button
        window.draw(quit);           // draw quit button
        window.draw(volume);         // draw volume button
        window.draw(manual);         // draw manual button
        window.draw(menu);           // draw menu button
        window.draw(overlay);        // then draw fading black overlay
        window.display();
    }
}


int main()
{
    // Videomode and RenderWindow class under sf namespace

    sf::VideoMode videoMode({1000, 1400}, 32);                          // setting the window size and bits
    sf::RenderWindow window(videoMode, "LEAP UP!", sf::Style::Default); // creates name for window and creates close, resize, and minimize button

    sf::Texture background, start, credits, quit, volume, mute,manual,start2,manual2,credits2,exit2,menu;

    if (!background.loadFromFile("assets/bg1.png") ||
        !start.loadFromFile("assets/play.png") ||
        !credits.loadFromFile("assets/credits.png") ||
        !quit.loadFromFile("assets/quit.png") ||
        !volume.loadFromFile("assets/volume.png") ||
        !mute.loadFromFile("assets/mute.png")||
        !start2.loadFromFile("assets/play2.png")||
        !manual.loadFromFile("assets/manual.png")||
        !manual2.loadFromFile("assets/manual2.png")||
        !credits2.loadFromFile("assets/credits2.png")||
        !exit2.loadFromFile("assets/exit2.png")||
        !menu.loadFromFile("assets/menu.png")) 
    {
        return -1; // error loading image
    }

    sf::Sprite bg(background);
    sf::Sprite Start(start);
    sf::Sprite Credit(credits);
    sf::Sprite Quit(quit);
    sf::Sprite Volume(volume);
    sf::Sprite Manual(manual);
    sf::Sprite Start2(start2);
    sf::Sprite Manual2(manual2);
    sf::Sprite Credits2(credits2);
    sf::Sprite Exit2(exit2);
    sf::Sprite Menu(menu); 

    sf::Music music;//music sprite
    if (!music.openFromFile("assets/music.mp3"))
    {
        std::cerr << "Failed to load music\n";
        return -1;
    }
    music.setLooping(true);//music loop
    music.play();
    

    bool musicOn = true;
    Menu.setPosition({275.f, 220.f}); // Set the position of the menu background
    Start.setPosition({240.f, 350.f});
    Manual.setPosition({240.f, 550.f});
    Credit.setPosition({240.f, 750.f});
    Quit.setPosition({240.f, 950.f});
    Start2.setPosition({240.f,350.f});
    
    Volume.setScale({0.2f, 0.2f});
    Menu.setScale({0.8f, 0.8f});
    Start.setScale({0.6f, 0.6f});
    Manual.setScale({0.6f, 0.6f});
    Credit.setScale({0.6f, 0.6f});
    Quit.setScale({0.6f, 0.6f});
    Start2.setScale({0.6f,0.6f});
    
    Scene currentScene = Scene::Menu;
    fadeIn(window, bg,Start,Credit,Quit,Volume, music,Manual,Menu); // Fade in effect

    
    
    while (window.isOpen())
    { // Loop to hold the window

        while (auto eventOpt = window.pollEvent())
        { // checks for user actions like closing window,mouse click
            if (!eventOpt.has_value())
                continue;

            const sf::Event &event = eventOpt.value();

            if (event.is<sf::Event::Closed>())
            {
                window.close();
            }
            
        

            //Hover effect for buttons
            if (event.is<sf::Event::MouseMoved>())
            {
                const auto &mouse = event.getIf<sf::Event::MouseMoved>();
                sf::Vector2f mousePos(static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y));
                

                //Start hover
                if (Start.getGlobalBounds().contains(mousePos))
                {
                    Start.setTexture(start2);//hover effect for start button
                }
                else{
                    Start.setTexture(start); // reset to original texture  
                }
                //manual hover    
                if (Manual.getGlobalBounds().contains(mousePos))
                {
                    Manual.setTexture(manual2); //hover effect for manual button
                }
                else{
                    Manual.setTexture(manual); // reset to original texture
                }
                //credits hover
                if (Credit.getGlobalBounds().contains(mousePos))
                {
                    Credit.setTexture(credits2); //hover effect for credits button
                }
                else{
                    Credit.setTexture(credits); // reset to original texture
                }
                //quit hover
                if (Quit.getGlobalBounds().contains(mousePos))
                {
                    Quit.setTexture(exit2); //hover effect for quit button
                }
                else{
                    Quit.setTexture(quit); // reset to original texture
                }
               
            }
            if (event.is<sf::Event::MouseButtonPressed>())
            {
                const auto &mouse = event.getIf<sf::Event::MouseButtonPressed>();
                sf::Vector2f mousePos(static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y));

                if (Quit.getGlobalBounds().contains(mousePos))
                {
                    return 0;
                }

                if (Start.getGlobalBounds().contains(mousePos)) {
                    currentScene = Scene::Game;
                    runGame(window); // Start the game
                }
                if(Manual.getGlobalBounds().contains(mousePos))
                {
                    currentScene = Scene::Manual;
                    runManual(window); // Open manual
                }



                if (Volume.getGlobalBounds().contains(mousePos))
                {

                    musicOn = !musicOn;
                    if (musicOn)
                    {
                        music.play();
                        Volume.setTexture(volume);
                    }
                    else
                    {
                        music.pause();
                        Volume.setTexture(mute);
                    }
                }
            }

             
            
            window.clear(sf::Color(0, 0, 0)); // color
            
            window.draw(bg);
            window.draw(Menu); 
            window.draw(Start);
            window.draw(Manual);
            window.draw(Credit);
            window.draw(Quit);
            window.draw(Volume);
            window.display(); // makes the window appear;used during gameloop
        }
    }
    return 0;

    
}








