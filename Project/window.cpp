//#include "game.hpp"
//#include "manual.hpp"
//#include "player.hpp"
//#include <SFML/Graphics.hpp>
//#include <SFML/Audio.hpp>
//#include <iostream>
//#include <ctime>
//#include <vector>
//
//enum class Scene
//{
//    Menu,
//    Game,
//    Manual
//};
//
//using namespace sf;
//using namespace std;
//
//void fadeIn(sf::RenderWindow& window, sf::Sprite& background, sf::Sprite& start, sf::Sprite& credits, sf::Sprite& quit,
//    sf::Sprite& volume, sf::Music& music, sf::Sprite& manual, sf::Sprite& menu)
//{
//    sf::RectangleShape overlay(sf::Vector2f(window.getSize()));
//    overlay.setFillColor(sf::Color(0, 0, 0, 255)); // Fully black
//
//    const float fadeDuration = 1.0f; // seconds
//    sf::Clock clock;
//
//    music.play(); // Start music just before fade-in
//
//    while (overlay.getFillColor().a > 0)
//    {
//        float elapsed = clock.getElapsedTime().asSeconds();
//        float alpha = 255 - (elapsed / fadeDuration) * 255;
//        if (alpha < 0)
//            alpha = 0;
//
//        overlay.setFillColor(sf::Color(0, 0, 0, static_cast<uint8_t>(alpha)));
//
//        window.clear();
//        window.draw(background); // draw background underneath
//        window.draw(start);      // draw start button
//        window.draw(credits);    // draw credits button
//        window.draw(quit);       // draw quit button
//        window.draw(volume);     // draw volume button
//        window.draw(manual);     // draw manual button
//        window.draw(menu);       // draw menu button
//        window.draw(overlay);    // then draw fading black overlay
//        window.display();
//    }
//}
//
//int main()
//{
//    // Videomode and RenderWindow class under sf namespace
//
//    VideoMode screen = VideoMode::getDesktopMode();
//    RenderWindow window(screen, "Leap up", Style::Default); // Not sf::State::Fullscreen
//    window.setPosition({ 0, 0 });                             // Place window at top-left
//    const float gameWidth = 1000.f;
//    const float gameHeight = 1400.f;
//    // View gameView(FloatRect(0,0, gameWidth, gameHeight));
//    View gameView(Vector2f(gameWidth / 2.f, gameHeight / 2.f),
//        Vector2f(gameWidth, gameHeight));
//
//    // Letterboxing logic
//    float windowRatio = static_cast<float>(screen.size.x) / screen.size.y;
//    float gameRatio = gameWidth / gameHeight;
//    float viewWidth = 1.f;
//    float viewHeight = 1.f;
//    float offsetX = 0.f;
//    float offsetY = 0.f;
//
//    // bars at sides
//    if (windowRatio > gameRatio)
//    {
//        viewWidth = gameRatio / windowRatio;
//        offsetX = (1.f - viewWidth) / 2.f;
//    }
//
//    // bars at top and bottom
//    else
//    {
//        viewHeight = windowRatio / gameRatio;
//        offsetY = (1.f - viewHeight) / 2.f;
//    }
//
//    gameView.setViewport(FloatRect(Vector2f(offsetX, offsetY), Vector2f(viewWidth, viewHeight)));
//    window.setView(gameView);
//
//    sf::Texture background, start, credits, quit, volume, mute, manual, start2, manual2, credits2, exit2, menu;
//
//    if (!background.loadFromFile("assets/bg1.png") ||
//        !start.loadFromFile("assets/play.png") ||
//        !credits.loadFromFile("assets/credits.png") ||
//        !quit.loadFromFile("assets/quit.png") ||
//        !volume.loadFromFile("assets/volume.png") ||
//        !mute.loadFromFile("assets/mute.png") ||
//        !start2.loadFromFile("assets/play2.png") ||
//        !manual.loadFromFile("assets/manual.png") ||
//        !manual2.loadFromFile("assets/manual2.png") ||
//        !credits2.loadFromFile("assets/credits2.png") ||
//        !exit2.loadFromFile("assets/exit2.png") ||
//        !menu.loadFromFile("assets/menu.png"))
//    {
//        return -1; // error loading image
//    }
//
//    sf::Sprite bg(background);
//    sf::Sprite Start(start);
//    sf::Sprite Credit(credits);
//    sf::Sprite Quit(quit);
//    sf::Sprite Volume(volume);
//    sf::Sprite Manual(manual);
//    sf::Sprite Start2(start2);
//    sf::Sprite Manual2(manual2);
//    sf::Sprite Credits2(credits2);
//    sf::Sprite Exit2(exit2);
//    sf::Sprite Menu(menu);
//
//
//
//
//
//
//
//    sf::Music music; // music sprite
//    if (!music.openFromFile("assets/music.mp3"))
//    {
//        std::cerr << "Failed to load music\n";
//        return -1;
//    }
//    music.setLooping(true); // music loop
//    music.play();
//
//    bool musicOn = true;
//    Menu.setPosition({ 275.f, 220.f }); // Set the position of the menu background
//    Start.setPosition({ 240.f, 350.f });
//    Manual.setPosition({ 240.f, 550.f });
//    Credit.setPosition({ 240.f, 750.f });
//    Quit.setPosition({ 240.f, 950.f });
//    Start2.setPosition({ 240.f, 350.f });
//
//    Volume.setScale({ 0.2f, 0.2f });
//    Menu.setScale({ 0.8f, 0.8f });
//    Start.setScale({ 0.6f, 0.6f });
//    Manual.setScale({ 0.6f, 0.6f });
//    Credit.setScale({ 0.6f, 0.6f });
//    Quit.setScale({ 0.6f, 0.6f });
//    Start2.setScale({ 0.6f, 0.6f });
//
//    Scene currentScene = Scene::Menu;
//    fadeIn(window, bg, Start, Credit, Quit, Volume, music, Manual, Menu); // Fade in effect
//
//    // Main application loop
//        // Main application loop
//    while (window.isOpen())
//    {
//        // Process all pending events in the event queue using std::optional.
//        // This is the recommended SFML 2.5+ / 3.x way to handle events.
//        while (auto eventOpt = window.pollEvent())
//        {
//            // Check if an event was actually retrieved
//            if (!eventOpt.has_value())
//                continue;
//
//            // Get a const reference to the actual event object from the optional
//            const sf::Event& event = eventOpt.value();
//
//            // Handle window
//            if (event.is<sf::Event::Closed>())
//            {
//                window.close();
//            }
//
//            // Only process mouse events if currently in the Menu scene
//            if (currentScene == Scene::Menu)
//            {
//                // CRITICAL: Convert mouse position from window coordinates to world coordinates.
//                // This is essential when using sf::View to correctly check sprite bounds,
//                // as mouse event coordinates are in window pixels, while sprites are in world units.
//                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
//
//                // Handle mouse movement for hover effects (SFML 3.x way)
//                if (event.is<sf::Event::MouseMoved>())
//                {
//                    // No need to getIf<MouseMoved> here, as mousePos is from sf::Mouse::getPosition()
//                    // and we only need to check bounds.
//                    // Check if mouse is over Start button and change texture accordingly
//                    if (Start.getGlobalBounds().contains(mousePos))
//                    {
//                        Start.setTexture(start2);
//                    }
//                    else
//                    {
//                        Start.setTexture(start);
//                    }
//                    // Check if mouse is over Manual button
//                    if (Manual.getGlobalBounds().contains(mousePos))
//                    {
//                        Manual.setTexture(manual2);
//                    }
//                    else
//                    {
//                        Manual.setTexture(manual);
//                    }
//                    // Check if mouse is over Credits button
//                    if (Credit.getGlobalBounds().contains(mousePos))
//                    {
//                        Credit.setTexture(credits2);
//                    }
//                    else
//                    {
//                        Credit.setTexture(credits);
//                    }
//                    // Check if mouse is over Quit button
//                    if (Quit.getGlobalBounds().contains(mousePos))
//                    {
//                        Quit.setTexture(exit2);
//                    }
//                    else
//                    {
//                        Quit.setTexture(quit);
//                    }
//                }
//                // Handle mouse button press events (SFML 3.x way)
//                else if (event.is<sf::Event::MouseButtonPressed>())
//                {
//                    // Access the MouseButtonPressed event data using getIf
//                    const auto* mouseButtonPressedEvent = event.getIf<sf::Event::MouseButtonPressed>();
//                        // Replace the problematic line with the following:
//                        if (mouseButtonPressedEvent && mouseButtonPressedEvent->button == sf::Mouse::Button::Left) // Check for left mouse button click
//                    {
//                        // Check which button was clicked and perform action
//                        if (Quit.getGlobalBounds().contains(mousePos))
//                        {
//                            window.close(); // Close the window if Quit button is clicked
//                        }
//                        else if (Start.getGlobalBounds().contains(mousePos))
//                        {
//                            currentScene = Scene::Game; // Change scene to Game
//                            // Call the game function. This function is assumed to run its own loop
//                            // and return control to main() when the game is over (e.g., player loses).
//                            runGame(window);
//                            // IMPORTANT: After runGame returns, reapply the menu view.
//                            // The game might have changed the window's view, so we need to reset it
//                            // for the menu to display correctly.
//                            window.setView(gameView);
//                            // Reset button textures to original state in case hover was active
//                            Start.setTexture(start);
//                            Manual.setTexture(manual);
//                            Credit.setTexture(credits);
//                            Quit.setTexture(quit);
//                            currentScene = Scene::Menu; // Return to menu scene after game ends
//                        }
//                        else if (Manual.getGlobalBounds().contains(mousePos))
//                        {
//                            currentScene = Scene::Manual; // Change scene to Manual
//                            // Call the manual function. Assumed to run its own loop and return.
//                            runManual(window);
//                            // Reapply the menu view after manual returns
//                            window.setView(gameView);
//                            // Reset button textures
//                            Start.setTexture(start);
//                            Manual.setTexture(manual);
//                            Credit.setTexture(credits);
//                            Quit.setTexture(quit);
//                            currentScene = Scene::Menu; // Return to menu scene after manual closes
//                        }
//                        else if (Volume.getGlobalBounds().contains(mousePos))
//                        {
//                            musicOn = !musicOn; // Toggle music state
//                            if (musicOn)
//                            {
//                                music.play();
//                                Volume.setTexture(volume); // Set volume on texture
//                            }
//                            else
//                            {
//                                music.pause();
//                                Volume.setTexture(mute); // Set volume off texture
//                            }
//                        }
//                        // The Credits button currently has no click action, but its hover effect will work.
//                    }
//                }
//            }
//        }
//
//
//
//
//
//         window.clear(sf::Color(0, 0, 0)); // Clear the window with black color
//
//          
//        if (currentScene == Scene::Menu)
//        {
//            window.draw(bg);     // Draw background image
//            window.draw(Menu);   // Draw the main menu background sprite
//            window.draw(Start);  // Draw the "Play" button
//            window.draw(Manual); // Draw the "Manual" button
//            window.draw(Credit); // Draw the "Credits" button
//            window.draw(Quit);   // Draw the "Quit" button
//            window.draw(Volume); // Draw the "Volume" toggle button
//        }
//
//        window.display();
//
//
//    }
//    return 0;
//}