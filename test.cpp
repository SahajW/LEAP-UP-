#include <SFML/Graphics.hpp>
#include "player.hpp"

int main() {
    sf::VideoMode videoMode({1000, 1400}, 32);                          // setting the window size and bits
    sf::RenderWindow window(videoMode, "LEAP UP!", sf::Style::Default);
    window.setFramerateLimit(60);

    Player player;
    sf::Clock clock;

    while (window.isOpen()) {
        
        while (auto eventOpt = window.pollEvent())
        { // checks for user actions like closing window,mouse click
            if (!eventOpt.has_value())
                continue;

            const sf::Event &event = eventOpt.value();

            if (event.is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        float dt = clock.restart().asSeconds();

        player.update(dt);

        // Simulate landing on platform:
        if (player.getPosition().y > 1000.f) {
            player.jump(); // pretend player hit a platform
        }

        window.clear(sf::Color(100, 149, 237)); // blue background
        player.draw(window);
        window.display();
    }

    return 0;
}
