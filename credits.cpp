#include "credits.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

enum class Scene
{
    Menu,
    Game,
    Manual,
    Credits
};

using namespace sf;
using namespace std;

void runCredits(sf::RenderWindow &window)
{
    VideoMode desktopMode = VideoMode::getDesktopMode();
    
    const sf::Vector2u windowSize(
        static_cast<unsigned int>((desktopMode.size.x / 2.56f)),
        static_cast<unsigned int>((desktopMode.size.y / 1.142857143f)));

    sf::Texture creditp, back;
    !creditp.loadFromFile("assets/creditsp.png"); // Load the manual image
    !back.loadFromFile("assets/back.png"); // Load the background image

    sf::Sprite Creditp(creditp);
    sf::Sprite Back(back);

    Back.setPosition({windowSize.x/1.582278481f, windowSize.y/1.186440678f});

    Scene currentScene = Scene::Credits;
    Back.setScale({windowSize.x / 2000.f, windowSize.y / 2800.f});
    while (window.isOpen())
    { // Loop to hold the window

        while (auto eventOpt = window.pollEvent())
        { // checks for user actions like closing window,mouse click
            if (!eventOpt.has_value())
                continue;

            const sf::Event &event = eventOpt.value();
            if (currentScene == Scene::Credits)
            {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                if (event.is<sf::Event::MouseButtonPressed>())
                {
                    const auto *mouseButtonPressedEvent = event.getIf<sf::Event::MouseButtonPressed>();
                    if (mouseButtonPressedEvent && mouseButtonPressedEvent->button == sf::Mouse::Button::Left) // Check for left mouse button click
                    {
                        if (Back.getGlobalBounds().contains(mousePos))
                        {
                            currentScene = Scene::Menu; // Change scene to Menu
                            return;                     // Return to menu
                        }
                    }

                }
                if (event.is<sf::Event::Closed>())
                {
                    window.close();
                }
                window.draw(Creditp);
                window.draw(Back);
                window.display();
            }
        }
    }
}