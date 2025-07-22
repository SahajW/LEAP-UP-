#include "over.hpp"
#include "game.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

enum class Scene { Menu, Game, Manual,Over };

using namespace sf;
using namespace std;

void runOver(sf::RenderWindow& window){

    sf::Texture Over,Again,Again2,Background;
    !Over.loadFromFile("assets/over.png");  
    !Again.loadFromFile("assets/again.png"); 
    !Again2.loadFromFile("assets/again.png"); 
    !Background.loadFromFile("assets/bg1.png"); 
    


    sf::Sprite over(Over),again(Again),again2(Again2),bg(Background);
    

    over.setPosition({240.f, 350.f});
    again.setPosition({240.f, 550.f});
    again2.setPosition({240.f, 550.f});

    Scene currentScene = Scene::Over;
    over.setScale({0.8f, 0.8f});
    again.setScale({0.7f, 0.7f});
    again2.setScale({0.7f, 0.7f});

    while (window.isOpen())
    { // Loop to hold the window

        while (auto eventOpt = window.pollEvent())
        { // checks for user actions like closing window,mouse click
            if (!eventOpt.has_value())
                continue;

            const sf::Event &event = eventOpt.value();
            if(currentScene ==Scene::Over){
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (event.is<sf::Event::MouseButtonPressed>())
            {
                const auto *mouseButtonPressedEvent = event.getIf<sf::Event::MouseButtonPressed>();
                if (mouseButtonPressedEvent && mouseButtonPressedEvent->button == sf::Mouse::Button::Left) // Check for left mouse button click
                {
                    if (again.getGlobalBounds().contains(mousePos))
                    {
                        currentScene = Scene::Game; // Change scene to Game
                        // Call the game function. This function is assumed to run its own loop
                        runGame(window);
                        return; // Return to game
                    }
                }
                
            }
            else if(event.is<sf::Event::MouseMoved>())
            {
                // Handle mouse movement for hover effects (SFML 3.x way)
                if (again.getGlobalBounds().contains(mousePos))
                {
                    again.setTexture(Again2);
                }
                else
                {
                    again.setTexture(Again);
                }
            }
            if (event.is<sf::Event::Closed>())
            {
                window.close();
            }
             window.draw(bg);
             window.draw(over);
             window.draw(again);
             window.draw(again2);
             window.display();
        }
        }
    }
}