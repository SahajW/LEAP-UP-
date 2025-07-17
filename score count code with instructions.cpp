//code for score count


//put this where the other values are initialized
int score = 0;                                  // <-- SCORE RELATED
float worldHeight = 0.f;                        // <-- SCORE RELATED

//initialize font font and add this text features.. put this after the values initialization
Text scoreText(font);
scoreText.setFillColor(Color::White);
scoreText.setStyle(Text::Bold);
scoreText.setCharacterSize(24);
scoreText.setPosition({ 10, 10 });

...
//game chaliraa bela, or when !gameOver, ko if statement ko body ma haalnu just add the diff and world height wala kura haru
if (b < h) {
    int diff = h - b;
    b = h;
    worldHeight += diff;                        // <-- SCORE RELATED
    for (int i = 0; i < platformCount; ++i) {
        plat[i].y += diff;
        if (plat[i].y > windowSize.y) {
            plat[i].y = 0;
            plat[i].x = rand() % (windowSize.x - platformWidth);
        }
    }
}

...
//yo pani when !gameOver ko body ma haalne, tara at the end end tira
score = static_cast<int>(worldHeight / 10);     // <-- SCORE RELATED
scoreText.setString("Score: " + to_string(score)); // <-- SCORE RELATED

...
//window draw player munni yo halne
window.draw(scoreText);                         // <-- SCORE RELATED


