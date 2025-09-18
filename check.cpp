#include "game.hpp"
#include "manual.hpp"
#include "credits.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include <vector>

enum class Scene
{
    Menu,
    Game,
    Manual,
    Credits
};

using namespace sf;
using namespace std;

void fadeIn(sf::RenderWindow &window, sf::Sprite &background, sf::Sprite &start, sf::Sprite &credits, sf::Sprite &quit,
            sf::Sprite &volume, sf::Music &music, sf::Sprite &manual, sf::Sprite &menu,sf::Sprite &next)
{
    sf::RectangleShape overlay(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 255)); // Fully black

    const float fadeDuration = 1.0f; // seconds
    sf::Clock clock;

    music.play(); // Start music just before fade-in

    while (overlay.getFillColor().a > 0)
    {
        float elapsed = clock.getElapsedTime().asSeconds();
        float alpha = 255 - (elapsed / fadeDuration) * 255;
        if (alpha < 0)
            alpha = 0;

        overlay.setFillColor(sf::Color(0, 0, 0, static_cast<uint8_t>(alpha)));

        window.clear();
        window.draw(background); // draw background underneath
        window.draw(start);      // draw start button
        window.draw(credits);    // draw credits button
        window.draw(quit);       // draw quit button
        window.draw(volume);     // draw volume button
        window.draw(manual);     // draw manual button
        window.draw(menu);       // draw menu button
        window.draw(next);       // draw next button
        window.draw(overlay);    // then draw fading black overlay
        window.display();
    }
}

int main()
{
    // Videomode and RenderWindow class under sf namespace
    VideoMode desktopMode = VideoMode::getDesktopMode();

    // Calculate window size as a percentage of screen size (e.g., 70% width, 60% height)
    const sf::Vector2u windowSize(
        static_cast<unsigned int>((desktopMode.size.x / 2.56f)),
        static_cast<unsigned int>((desktopMode.size.y / 1.142857143f)));
    ////////////////////////////////////////////////////////////////////////////////////////////////////

    RenderWindow window(VideoMode({windowSize.x, windowSize.y}), "Leap UP", Style::Titlebar | Style::Close);

    sf::Texture background, start, credits, quit, volume, mute, manual, start2, manual2, credits2, exit2, menu, next;

    if (!background.loadFromFile("assets/bg1.png") ||
        !start.loadFromFile("assets/play.png") ||
        !credits.loadFromFile("assets/credits.png") ||
        !quit.loadFromFile("assets/quit.png") ||
        !volume.loadFromFile("assets/volume.png") ||
        !mute.loadFromFile("assets/mute.png") ||
        !start2.loadFromFile("assets/play2.png") ||
        !manual.loadFromFile("assets/manual.png") ||
        !manual2.loadFromFile("assets/manual2.png") ||
        !credits2.loadFromFile("assets/credits2.png") ||
        !exit2.loadFromFile("assets/exit2.png") ||
        !menu.loadFromFile("assets/menu.png") ||
        !next.loadFromFile("assets/next.png"))
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
    sf::Sprite Next(next);

    sf::Music music1; // music sprite
    if (!music1.openFromFile("assets/music.mp3"))
    {
        std::cerr << "Failed to load music\n";
        return -1;
    }
    music1.setLooping(true); // music loop
    music1.play();
    sf::Music music2;

    if (!music2.openFromFile("assets/music2.mp3"))
    {
        std::cerr << "Failed to load music2\n";
        return -1;
    }
    music2.setLooping(true); // music2 loop

    sf::Music music3;
    if (!music3.openFromFile("assets/music1.mp3"))
    {
        std::cerr << "Failed to load music3\n";
        return -1;
    }
    music3.setLooping(true);

    sf::Music music4;
    if (!music4.openFromFile("assets/music4.mp3"))
    {
        std::cerr << "Failed to load music4\n";
        return -1;
    }
    music4.setLooping(true);

    sf::Music mouse;
    if (!mouse.openFromFile("assets/mouse.wav"))
    {
        std::cerr << "Failed to load mouse sound\n";
        return -1;
    }

    mouse.setLooping(false); // mouse sound not looping

    bool musicOn = true;
    bool nextmusic = true;
    // Menu.setPosition({275.f, 220.f}); // Set the position of the menu background
    Menu.setPosition({windowSize.x / 3.636363636f, windowSize.y / 6.363636364f});
    // Start.setPosition({240.f, 350.f});
    Start.setPosition({windowSize.x / 4.166666667f, windowSize.y / 4.f});
    // Manual.setPosition({240.f, 550.f});
    Manual.setPosition({windowSize.x / 4.166666667f, windowSize.y / 2.545454545f});
    // Credit.setPosition({240.f, 750.f});
    Credit.setPosition({windowSize.x / 4.166666667f, windowSize.y / 1.866666667f});
    // Quit.setPosition({240.f, 950.f});
    Quit.setPosition({windowSize.x / 4.166666667f, windowSize.y / 1.473684211f});
    // Start2.setPosition({240.f, 350.f});
    Start2.setPosition({windowSize.x / 4.166666667f, windowSize.y / 4.f});
    Next.setPosition({windowSize.x / 7.5f, windowSize.y / 90.f});

    // Volume.setScale({0.2f, 0.2f});
    Volume.setScale({windowSize.x / 5000.f, windowSize.y / 7000.f});
    // Menu.setScale({0.8f, 0.8f});
    Menu.setScale({windowSize.x / 1250.f, windowSize.y / 1750.f});
    // Start.setScale({0.6f, 0.6f});
    Next.setScale({windowSize.x / 7000.f, windowSize.y / 9800.f});
    Start.setScale({windowSize.x / 1666.666667f, windowSize.y / 2333.333333f});
    Manual.setScale({windowSize.x / 1666.666667f, windowSize.y / 2333.333333f});
    Credit.setScale({windowSize.x / 1666.666667f, windowSize.y / 2333.333333f});
    Quit.setScale({windowSize.x / 1666.666667f, windowSize.y / 2333.333333f});
    Start2.setScale({windowSize.x / 1666.666667f, windowSize.y / 2333.333333f});

    Scene currentScene = Scene::Menu;
    fadeIn(window, bg, Start, Credit, Quit, Volume, music1, Manual, Menu, Next); // Fade in effect

    // Main application loop
    // Main application loop
    while (window.isOpen())
    {
        // Process all pending events in the event queue using std::optional.
        // This is the recommended SFML 2.5+ / 3.x way to handle events.
        while (auto eventOpt = window.pollEvent())
        {
            // Check if an event was actually retrieved
            if (!eventOpt.has_value())
                continue;

            // Get a const reference to the actual event object from the optional
            const sf::Event &event = eventOpt.value();

            // Handle window
            if (event.is<sf::Event::Closed>())
            {
                window.close();
            }

            // Only process mouse events if currently in the Menu scene
            if (currentScene == Scene::Menu)
            {
                // CRITICAL: Convert mouse position from window coordinates to world coordinates.
                // This is essential when using sf::View to correctly check sprite bounds,
                // as mouse event coordinates are in window pixels, while sprites are in world units.
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

                // Handle mouse movement for hover effects (SFML 3.x way)
                if (event.is<sf::Event::MouseMoved>())
                {
                    // No need to getIf<MouseMoved> here, as mousePos is from sf::Mouse::getPosition()
                    // and we only need to check bounds.
                    // Check if mouse is over Start button and change texture accordingly
                    if (Start.getGlobalBounds().contains(mousePos))
                    {
                        Start.setTexture(start2);
                    }
                    else
                    {
                        Start.setTexture(start);
                    }
                    // Check if mouse is over Manual button
                    if (Manual.getGlobalBounds().contains(mousePos))
                    {
                        Manual.setTexture(manual2);
                    }
                    else
                    {
                        Manual.setTexture(manual);
                    }
                    // Check if mouse is over Credits button
                    if (Credit.getGlobalBounds().contains(mousePos))
                    {
                        Credit.setTexture(credits2);
                    }
                    else
                    {
                        Credit.setTexture(credits);
                    }
                    // Check if mouse is over Quit button
                    if (Quit.getGlobalBounds().contains(mousePos))
                    {
                        Quit.setTexture(exit2);
                    }
                    else
                    {
                        Quit.setTexture(quit);
                    }
                }
                // Handle mouse button press events (SFML 3.x way)
                else if (event.is<sf::Event::MouseButtonPressed>())
                {
                    // Access the MouseButtonPressed event data using getIf
                    const auto *mouseButtonPressedEvent = event.getIf<sf::Event::MouseButtonPressed>();
                    // Replace the problematic line with the following:
                    if (mouseButtonPressedEvent && mouseButtonPressedEvent->button == sf::Mouse::Button::Left) // Check for left mouse button click
                    {
                        mouse.play(); // Play mouse click sound
                        // Check which button was clicked and perform action
                        if (Quit.getGlobalBounds().contains(mousePos))
                        {
                            window.close(); // Close the window if Quit button is clicked
                        }
                        else if (Start.getGlobalBounds().contains(mousePos))
                        {
                            currentScene = Scene::Game; // Change scene to Game
                            // Call the game function. This function is assumed to run its own loop
                            // and return control to main() when the game is over (e.g., player loses).
                            music1.stop();
                            music3.stop();
                            music2.stop();
                            music4.stop();

                            if (nextmusic)
                                music2.play(); // If menu was playing music1, play music2
                            else
                                music4.play(); // If menu was playing music3, play music4

                            runGame(window);
                            // IMPORTANT: After runGame returns, reapply the menu view.
                            // The game might have changed the window's view, so we need to reset it
                            // for the menu to display correctly.
                            music2.stop();
                            music4.stop();
                            if (musicOn)
                            {
                                if (nextmusic)
                                    music1.play();
                                else
                                    music3.play();
                            }
                            // window.setView(gameView);
                            //  Reset button textures to original state in case hover was active
                            Start.setTexture(start);
                            Manual.setTexture(manual);
                            Credit.setTexture(credits);
                            Quit.setTexture(quit);
                            currentScene = Scene::Menu; // Return to menu scene after game ends
                        }
                        else if (Manual.getGlobalBounds().contains(mousePos))
                        {
                            currentScene = Scene::Manual; // Change scene to Manual
                            // Call the manual function. Assumed to run its own loop and return.
                            runManual(window);
                            // Reapply the menu view after manual returns
                            // window.setView(gameView);
                            // Reset button textures
                            Start.setTexture(start);
                            Manual.setTexture(manual);
                            Credit.setTexture(credits);
                            Quit.setTexture(quit);
                            currentScene = Scene::Menu; // Return to menu scene after manual closes
                        }
                         else if (Credit.getGlobalBounds().contains(mousePos))
                        {
                            currentScene = Scene::Credits; // Change scene to Manual
                            // Call the manual function. Assumed to run its own loop and return.
                            runCredits(window);
                            // Reapply the menu view after manual returns
                            // window.setView(gameView);
                            // Reset button textures
                            Start.setTexture(start);
                            Manual.setTexture(manual);
                            Credit.setTexture(credits);
                            Quit.setTexture(quit);
                            currentScene = Scene::Menu; // Return to menu scene after manual closes
                        }
                        
                        else if (Volume.getGlobalBounds().contains(mousePos))
                        {
                            musicOn = !musicOn; // Toggle music state
                            if (musicOn)
                            {
                                if (nextmusic)
                                {
                                    music1.play();
                                    music2.stop(); // optional: ensure game music isn’t still playing
                                }
                                else
                                {
                                    music3.play();
                                    music4.stop();
                                }
                                Volume.setTexture(volume);
                            }
                            else
                            {
                                if (nextmusic)
                                {
                                    music1.pause();
                                    music2.pause();
                                }
                                else
                                {
                                    music3.pause();
                                    music4.pause();
                                }
                                Volume.setTexture(mute);
                            }
                        }
                        else if (Next.getGlobalBounds().contains(mousePos))
                        {
                            if (nextmusic)
                            {
                                music1.stop();
                                music3.play();
                                nextmusic = false;
                            }
                            else
                            {
                                music3.stop();
                                music1.play();
                                nextmusic = true;
                            }
                        }
                        // The Credits button currently has no click action, but its hover effect will work.
                    }
                }
            }
        }

        window.clear(sf::Color(0, 0, 0)); // Clear the window with black color

        if (currentScene == Scene::Menu)
        {
            window.draw(bg);     // Draw background image
            window.draw(Menu);   // Draw the main menu background sprite
            window.draw(Start);  // Draw the "Play" button
            window.draw(Manual); // Draw the "Manual" button
            window.draw(Credit); // Draw the "Credits" button
            window.draw(Quit);   // Draw the "Quit" button
            window.draw(Volume); // Draw the "Volume" toggle button
            window.draw(Next);   
        
        }

        window.display();
    }
    return 0;
}