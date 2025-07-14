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

    Back.setPosition({629.f, 1179.f});

    Scene currentScene = Scene::Manual;
    Back.setScale({0.5f, 0.5f});
    while (window.isOpen())
    { // Loop to hold the window

        while (auto eventOpt = window.pollEvent())
        { // checks for user actions like closing window,mouse click
            if (!eventOpt.has_value())
                continue;

            const sf::Event &event = eventOpt.value();

            if (event.is<sf::Event::MouseButtonPressed>())
            {
                const auto &mouse = event.getIf<sf::Event::MouseButtonPressed>();
                sf::Vector2f mousePos(static_cast<float>(mouse->position.x), static_cast<float>(mouse->position.y));

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