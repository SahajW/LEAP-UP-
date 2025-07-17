//WIH GAMEOVER, SCORE COUNT, HIGHSCORE, PAUSE
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <vector>
#include <Windows.h>
#include <fstream>

using namespace sf;
using namespace std;

int main() {
    // === Load font ===
    Font font;
    if (!font.openFromFile("roboto.ttf")) {
        throw runtime_error("Could not load font");
    }

    // === Score & Highscore Setup ===
    int score = 0;
    int highscore = 0;

    ifstream inputFile("highscore.txt");
    if (inputFile.is_open()) {
        inputFile >> highscore;
        inputFile.close();
    }

    Text scoreText(font);
    scoreText.setFillColor(Color::White);
    scoreText.setStyle(Text::Bold);
    scoreText.setCharacterSize(24);
    scoreText.setPosition({10, 10});

    Text highscoreText(font);
    highscoreText.setFillColor(Color::White);
    highscoreText.setStyle(Text::Bold);
    highscoreText.setCharacterSize(24);
    highscoreText.setPosition({250, 10});
    highscoreText.setString("High Score: " + to_string(highscore));

    Text gameOverText(font);
    gameOverText.setFillColor(Color::Red);
    gameOverText.setStyle(Text::Bold);
    gameOverText.setCharacterSize(36);
    gameOverText.setString("GAME OVER");
    gameOverText.setPosition({10.f, 60.f});

    // === Screen Size ===
    VideoMode desktopMode = VideoMode::getDesktopMode();
    const sf::Vector2u windowSize(
        static_cast<unsigned int>(desktopMode.size.x / 3.2f),
        static_cast<unsigned int>(desktopMode.size.y / 1.3f)
    );

   RenderWindow window(VideoMode({ windowSize.x, windowSize.y }), "Leap UP");
    window.setFramerateLimit(60);

    // === Pause Button Setup ===
    Texture pauseTexture;
    if (!pauseTexture.loadFromFile("pause.png")) {
        cout << "Could not load pause.png" << endl;
        return -1;
    }
    Sprite pauseButton(pauseTexture);
pauseButton.setPosition({windowSize.x / 2.f - 50.f, 10}); // top-right corner
pauseButton.setScale({0.5f, 0.5f}); // resize if needed

    bool isPaused = false;

    // === Game Setup ===
    float worldHeight = 0.f;
    const int platformCount = 7;
    const int platformWidth = 60;
    const int platformHeight = 15;
    const int playerWidth = 30;
    const int playerHeight = 30;

    Texture platformTexture, playerTexture, bgTexture;
    if (!platformTexture.loadFromFile("plat.png", false, IntRect({10, 10}, {platformWidth, platformHeight})) ||
        !playerTexture.loadFromFile("plat.png", false, IntRect({0, 0}, {playerWidth, playerHeight}))) {
        cout << "Error loading textures" << endl;
        return -1;
    }

    Sprite platform(platformTexture), player(playerTexture), bg(bgTexture);

    Vector2f plat[platformCount];
    for (int i = 0; i < platformCount; ++i) {
        plat[i].x = static_cast<float>(rand() % windowSize.x);
        plat[i].y = i * (windowSize.y / platformCount);
    }

   player.setOrigin({ 15.f, 15.f });

    const float moveSpeed = 3.f;
    const float playerJumpSpeed = 10.f;
    const float gravity = 0.2f;
    float a, b, h = 200;
    float da = 0, db = 0;

    int midIndex = platformCount / 2;
    float platformX = plat[midIndex].x;
    float platformY = plat[midIndex].y;

    a = platformX + platformWidth / 2.f;
    b = platformY - playerHeight / 2.f;

    bool gameOver = false;

    // === Main Game Loop ===
   
       
    while (window.isOpen())
    {
        while (auto event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        
        else if (event->is<sf::Event::MouseButtonPressed>()) {
        Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));
        if (pauseButton.getGlobalBounds().contains(mousePos)) {
            isPaused = !isPaused;
        }
    }

            else if (event->is<Event::KeyPressed>())
            {
        if (Keyboard::isKeyPressed(Keyboard::Key::P)) 
        {
            isPaused = !isPaused;
            
        }
    }
}


        if (!gameOver && !isPaused) {
           if (Keyboard::isKeyPressed(Keyboard::Key::Right)) a += moveSpeed;
            if (Keyboard::isKeyPressed(Keyboard::Key::Left)) a -= moveSpeed;

            db += 0.2f;
            b += db;

            if (b < h) {
                int diff = static_cast<int>(h - b);
                b = h;
                worldHeight += diff;
                for (int i = 0; i < platformCount; ++i) {
                    plat[i].y += diff;
                    if (plat[i].y > windowSize.y) {
                        plat[i].y = 0;
                        plat[i].x = rand() % (windowSize.x - platformWidth);
                    }
                }
            }

            for (int i = 0; i < platformCount; ++i) {
                if (a + playerWidth / 2.f > plat[i].x &&
                    a - playerWidth / 2.f < plat[i].x + platformWidth &&
                    b + playerHeight / 2.f > plat[i].y &&
                    b + playerHeight / 2.f < plat[i].y + platformHeight &&
                    db > 0) {
                    db = -10.f;
                    b = plat[i].y - playerHeight / 2.f;
                }
            }

            if (b - playerHeight / 2.f > windowSize.y)
                gameOver = true;

            score = static_cast<int>(worldHeight / 50);
            scoreText.setString("Score: " + to_string(score));

            if (score > highscore) {
                highscore = score;
                highscoreText.setString("High Score: " + to_string(highscore));
            }
        }

        player.setPosition({a, b});

        // === Render Frame ===
        window.clear();
        window.draw(bg);
        for (int i = 0; i < platformCount; ++i) {
            platform.setPosition(plat[i]);
            window.draw(platform);
        }
        window.draw(player);
        window.draw(scoreText);
        window.draw(highscoreText);
        window.draw(pauseButton);
        if (gameOver)
            window.draw(gameOverText);
        window.display();
    }

    // === Save Highscore ===
    ofstream outputFile("highscore.txt");

    if (outputFile.is_open()) {
        outputFile << highscore;
        outputFile.close();
    }

    return 0;
}
