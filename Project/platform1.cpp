//#include <SFML/Graphics.hpp>
//#include <iostream>
//#include <ctime>
//#include <vector>
//#include <SFML/Graphics/Rect.hpp>
//#include <cstdlib>  // For rand(), srand()
//
//using namespace sf;
//using namespace std;
//
//enum class PlatformType {
//    NORMAL,   //0
//    DISSAPEARING,   //1
//    MOVING,   //2
//    THORN   //3
//};
//
//struct Platform {
//    RectangleShape shape;
//    Vector2f velocity; // for moving platforms
//    PlatformType type;
//    bool visible = true;
//};
//
//vector<Platform> platforms;
//
//
//int main()
//{
//    srand(static_cast<unsigned>(time(0)));
//
//    /*
//            Window Creation
//
//    */
//
//
//    /*1 fullscreen*/
//
//    VideoMode screen = VideoMode::getDesktopMode();
//    RenderWindow window(screen, "Leap up", Style::Default);  // Not sf::State::Fullscreen
//    window.setPosition({ 0, 0 });  // Place window at top-left
//
//
//    window.setFramerateLimit(60);
//
//
//    /*
//
//        background texture
//    */
//
//
//    Texture texture;
//    if (!texture.loadFromFile("sprite/bg1.png"))
//    {
//        cout << "Failed to load background texture!" << endl;
//        return -1;
//    }
//    Sprite sprite(texture);
//
//    const float gameWidth = 1000.f;
//    const float gameHeight = 1400.f;
//    //View gameView(FloatRect(0,0, gameWidth, gameHeight));
//    View gameView(Vector2f(gameWidth / 2.f, gameHeight / 2.f),
//        Vector2f(gameWidth, gameHeight));
//
//
//
//
//    /*
//
//            blackbars on the sides
//
//    */
//
//
//
//    // Letterboxing logic
//    float windowRatio = static_cast<float>(screen.size.x) / screen.size.y;
//    float gameRatio = gameWidth / gameHeight;
//    float viewWidth = 1.f;
//    float viewHeight = 1.f;
//    float offsetX = 0.f;
//    float offsetY = 0.f;
//
//
//    //bars at sides
//    if (windowRatio > gameRatio) {
//        viewWidth = gameRatio / windowRatio;
//        offsetX = (1.f - viewWidth) / 2.f;
//    }
//
//    //bars at top and bottom    
//    else {
//        viewHeight = windowRatio / gameRatio;
//        offsetY = (1.f - viewHeight) / 2.f;
//    }
//
//    gameView.setViewport(FloatRect(Vector2f(offsetX, offsetY), Vector2f(viewWidth, viewHeight)));
//    window.setView(gameView);
//
//
//
//
//
//
//
//
//    /*
//            Dalla ko test game
//    */
//
//
//    const int platformCount = 15;
//    const float platformWidth = 100.f;
//    const float platformHeight = 25.f;
//
//    // Platforms
//    vector<RectangleShape> plat(platformCount);
//    vector<Vector2f> opos(platformCount); // Store positions for scrolling
//
//    //for (int i = 0; i < platformCount; ++i)
//    //{
//    //    plat[i].setSize(Vector2f(platformWidth, platformHeight));
//    //    plat[i].setFillColor(Color::Green);
//    //    float x = rand() % 900; // Keep inside screen width (400 - 60)
//    //    float y = i * (1400 / platformCount); // evenly spaced
//    //    plat[i].setPosition({ x, y });
//    //    opos[i] = { x, y };
//    //}
// 
// 
//    vector<Platform> platforms(platformCount);
//
// 
//
//    std::vector<Platform> platforms(platformCount);
//    for (int i = 0; i < platformCount; ++i)
//    {
//        platforms[i].shape.setSize(Vector2f(platformWidth, platformHeight));
//
//        int typeRoll = rand() % 100; // 0-99
//        if (typeRoll < 70) { // 70% chance for normal platforms
//            platforms[i].type = NORMAL;
//            platforms[i].shape.setFillColor(Color::Green); // Fallback color
//            if (normalPlatformTexture.isSmooth()) platforms[i].shape.setTexture(&normalPlatformTexture);
//        }
//        else if (typeRoll < 85) { // 15% chance for thorn platforms
//            platforms[i].type = THORN;
//            platforms[i].shape.setFillColor(Color::Red); // Fallback color
//            if (thornPlatformTexture.isSmooth()) platforms[i].shape.setTexture(&thornPlatformTexture);
//        }
//        else { // 15% chance for moving platforms
//            platforms[i].type = MOVING;
//            platforms[i].shape.setFillColor(Color::Blue); // Fallback color
//            if (movingPlatformTexture.isSmooth()) platforms[i].shape.setTexture(&movingPlatformTexture);
//            platforms[i].moveDirection = (rand() % 2 == 0) ? 1.f : -1.f; // Random initial direction (right or left)
//            platforms[i].moveSpeed = (rand() % 2 + 1) * 1.5f; // Speed between 1.5 and 3.0
//        }
//
//
//        float x = rand() % static_cast<int>(gameWidth - platformWidth);
//        float y = i * (gameHeight / platformCount);
//
//        platforms[i].shape.setPosition(Vector2f(x, y));
//       platforms[i].type = (i % 5 == 0) ? PlatformType::Disappearing : PlatformType::Normal;  // Example: 1 in 5 are disappearing
//    }
//
//
//
//
//
//
//    // Player
//    const float playerRadius = 15.f;
//    CircleShape player(playerRadius);
//    player.setFillColor(Color::Yellow);
//    player.setOrigin({ playerRadius, playerRadius });
//
//    float a, b, h = 200; // a = x, b = y
//    float da = 0, db = 0;
//
//    // Start player on middle platform 
//    int midIndex = platformCount / 2;
//    float platformX = plat[midIndex].getPosition().x;
//    float platformY = plat[midIndex].getPosition().y;
//
//    a = platformX + platformWidth / 2.f;         // horizontally centered
//    b = platformY - playerRadius;                // standing on top
//
//
//    while (window.isOpen())
//    {
//        while (auto event = window.pollEvent())
//        {
//            if (event->is<Event::Closed>())
//            {
//                window.close();
//            }
//        }
//
//        // Movement input
//        if (Keyboard::isKeyPressed(Keyboard::Key::Right)) a += 3;
//        if (Keyboard::isKeyPressed(Keyboard::Key::Left)) a -= 3;
//
//        // Gravity and fall
//        db += 0.2f;
//        b += db;
//
//        // Game Over if player falls below screen
//        if (b - playerRadius > window.getSize().y)
//        {
//            cout << "Game Over!" << endl;
//            window.close();
//        }
//
//        // Scroll world if above height
//        if (b < h)
//        {
//            b = h;
//            for (int i = 0; i < platformCount; ++i)
//            {
//                opos[i].y += -db;
//                if (opos[i].y > 1400)
//                {
//                    opos[i].y = 0;
//                    opos[i].x = rand() % 900;
//                }
//                plat[i].setPosition(opos[i]);
//            }
//        }
//
//
//        //for (int i = 0; i < platformCount; ++i)
//        //{
//        //    auto& plat = platforms[i];
//        //    sf::Vector2f pos = plat.shape.getPosition();
//        //    pos.y += -db;
//
//        //    if (pos.y > gameHeight) {
//        //        pos.y = 0;
//        //        pos.x = rand() % static_cast<int>(gameWidth - platformWidth);
//        //        plat.visible = true;
//
//        //        // This is where your new code goes:
//        //        int r = rand() % 10;
//        //        if (r < 2) plat.type = PlatformType::Disappearing;
//        //        else if (r == 2) {
//        //            plat.type = PlatformType::Thorn;
//        //            plat.shape.setFillColor(sf::Color::Red); // Set color for Thorn platforms
//        //        }
//        //        else plat.type = PlatformType::Normal;
//
//        //    }
//
//        //    plat.shape.setPosition(pos);
//        //}
//
//
//
//
//        // Collision with platforms
//        Vector2f pPos = { a, b };
//
//        for (int i = 0; i < platformCount; ++i)
//        {
//            Vector2f platPos = plat[i].getPosition();
//
//            if (pPos.x + playerRadius > platPos.x &&
//                pPos.x - playerRadius < platPos.x + platformWidth &&
//                pPos.y + playerRadius > platPos.y &&
//                pPos.y + playerRadius < platPos.y + platformHeight &&
//                db > 0)
//            {
//                db = -10;
//                b = platPos.y - playerRadius;
//            }
//        }
//
//
//
//
//        /*
//                player rigth-left , left to right reappear
//
//        */
//
//
//
//        // sanchit ko code 
//
//        /*if (pPos.x > gameWidth + playerRadius) {
//            a = -playerRadius;
//            b = pPos.y;
//        }
//
//
//
//        else if (pPos.x > gameWidth + playerRadius) {
//            a = gameWidth + playerRadius;
//            b = pPos.y;
//
//
//        }*/
//
//
//
//        if (pPos.x > gameWidth + playerRadius) {
//            a = 0; // Reappear just inside left edge
//            b = pPos.y;
//        }
//        else if (pPos.x < 0) {
//            a = gameWidth; // Reappear just inside right edge
//            b = pPos.y;
//        }
//        player.setPosition({ a, b });
//
//
//
//
//        window.clear(Color::Black);
//
//
//        window.setView(gameView);
//
//
//
//        window.draw(sprite); // Draw background sprite
//
//
//        for (int i = 0; i < platformCount; ++i)
//            window.draw(plat[i]);
//        window.draw(player);
//
//
//        window.display();
//    }
//    return 0;
//}