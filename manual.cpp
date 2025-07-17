#include "manual.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

enum class Scene { Menu, Game, Manual };

using namespace sf;
using namespace std;

void runManual(sf::RenderWindow& window){

    sf::Texture manualp,back;
    !manualp.loadFromFile("assets/manualp.png");  // Load the manual image
    !back.loadFromFile("assets/back.png"); // Load the background image


    sf::Sprite Manualp(manualp);
    sf::Sprite Back(back);

    Back.setPosition({632.f, 1180.f});

    Scene currentScene = Scene::Manual;
    Back.setScale({0.5f, 0.5f});
    while (window.isOpen())
    { // Loop to hold the window

        while (auto eventOpt = window.pollEvent())
        { // checks for user actions like closing window,mouse click
            if (!eventOpt.has_value())
                continue;

            const sf::Event &event = eventOpt.value();
            if(currentScene ==Scene::Manual){
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (event.is<sf::Event::MouseButtonPressed>())
            {
                const auto *mouseButtonPressedEvent = event.getIf<sf::Event::MouseButtonPressed>();
                if (mouseButtonPressedEvent && mouseButtonPressedEvent->button == sf::Mouse::Button::Left) // Check for left mouse button click
                {
                    if (Back.getGlobalBounds().contains(mousePos))
                    {
                        currentScene = Scene::Menu; // Change scene to Menu
                        return; // Return to menu
                    }
                }
                

                if (Back.getGlobalBounds().contains(mousePos))
                {
                    currentScene = Scene::Menu;
                    return; // Return to menu
                    
                }
            }
            if (event.is<sf::Event::Closed>())
            {
                window.close();
            }
             window.draw(Manualp);
             window.draw(Back);
             window.display();
        }
        }
    }
}