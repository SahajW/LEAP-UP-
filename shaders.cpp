//WITH SHADERS HAI WORKING GAME
#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <vector>
using namespace std;
using namespace sf;

void showGameOverScreen(RenderWindow& gameWindow) {
    const int windowWidth = 400;
    const int windowHeight = 600;

    RenderWindow window(VideoMode({windowWidth, windowHeight}), "Game Over");

    Font font;
    if (!font.openFromFile("roboto.ttf")) {
        throw runtime_error("Could not load font");
    }

    Texture bgTexture;
    if (!bgTexture.loadFromFile("GAMEOVERbg.png")) {
        throw runtime_error("Could not load background image");
    }
    Sprite background(bgTexture);
// Game Over text setup
    Text text(font);
    text.setFillColor(Color::White);
    text.setString("GAME OVER");
    text.setStyle(Text::Bold);
    text.setCharacterSize(50);
    text.setPosition({0.f, 200.f});  // Start from left edge
    // GLSL Fragment Shader (Glowing Red Pulse)
    Shader shader;
    const string fragmentShader = R"(
    uniform float time;
    uniform sampler2D texture; // <- this is important!

    void main()
    {
        vec4 pixel = texture2D(texture, gl_TexCoord[0].xy); // Sample the actual text color
        float glow = abs(sin(time * 3.0));
        gl_FragColor = pixel * vec4(glow, glow, glow, 1.0); // Brighten it with time
    }
)";


   if (!shader.loadFromMemory(fragmentShader, Shader::Type::Fragment)) {
    throw runtime_error("Failed to load shader from memory");
}

    Clock clock;

    while (window.isOpen()) {
        while (optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }

        float time = clock.getElapsedTime().asSeconds();
        shader.setUniform("time", time);

        window.clear();
        window.draw(background);
        window.draw(text, &shader);  // Apply shader to text
        window.display();
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    RenderWindow window(VideoMode({400, 600}), "Doodle Jump Clone");
    window.setFramerateLimit(60);

    const int platformCount = 10;
    const float platformWidth = 60.f;
    const float platformHeight = 15.f;

    vector<RectangleShape> plat(platformCount);
    vector<Vector2f> opos(platformCount); // Store positions for scrolling

    for (int i = 0; i < platformCount; ++i) {
        plat[i].setSize(Vector2f(platformWidth, platformHeight));
        plat[i].setFillColor(Color::Green);
        float x = rand() % 340;
        float y = i * (533 / platformCount);
        plat[i].setPosition({x, y});
        opos[i] = {x, y};
    }

    // Player setup
    const float playerRadius = 15.f;
    CircleShape player(playerRadius);
    player.setFillColor(Color::Yellow);
    player.setOrigin({playerRadius, playerRadius});

    float a, b, h = 200;
    float da = 0, db = 0;

    int midIndex = platformCount / 2;
    float platformX = plat[midIndex].getPosition().x;
    float platformY = plat[midIndex].getPosition().y;

    a = platformX + platformWidth / 2.f;
    b = platformY - playerRadius;

    while (window.isOpen()) {
        while (optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
                return 0;
            }
        }

        // Movement
        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) a += 3;
        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) a -= 3;

        // Gravity
        db += 0.2f;
        b += db;

        // Game Over if player falls below screen
        if (b - playerRadius > window.getSize().y) {
            window.close();               // Close game window
            showGameOverScreen(window);   // Open Game Over screen with shader
            return 0;
        }

        // Scroll world
        if (b < h) {
            b = h;
            for (int i = 0; i < platformCount; ++i) {
                opos[i].y += -db;
                if (opos[i].y > 533) {
                    opos[i].y = 0;
                    opos[i].x = rand() % 340;
                }
                plat[i].setPosition(opos[i]);
            }
        }

        // Collision
        for (int i = 0; i < platformCount; ++i) {
            Vector2f pPos = {a, b};
            Vector2f platPos = plat[i].getPosition();

            if (pPos.x + playerRadius > platPos.x &&
                pPos.x - playerRadius < platPos.x + platformWidth &&
                pPos.y + playerRadius > platPos.y &&
                pPos.y + playerRadius < platPos.y + platformHeight &&
                db > 0)
            {
                db = -10;
                b = platPos.y - playerRadius;
            }
        }

        player.setPosition({a, b});

        window.clear(Color::Black);
        window.draw(player);
        for (int i = 0; i < platformCount; ++i)
            window.draw(plat[i]);
        window.display();
    }

    return 0;
}
