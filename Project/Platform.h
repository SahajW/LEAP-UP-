#pragma once // Using pragma once

#include <SFML/Graphics.hpp> // Required for sf::RectangleShape, sf::Vector2f, sf::Color
#include <cstdlib>           // For rand() in Platform initialization

// Define an enum for platform types
enum PlatformType {
    NORMAL,
    THORN,
    MOVING,
    DISAPPEARING
};

// Defining a class to hold all platform properties and behavior
class Platform {
public:
    sf::RectangleShape shape;
    sf::Vector2f originalPosition;  // Stores the base position for scrolling
    PlatformType type;
    float moveDirection;            // 1 for right, -1 for left
    float moveSpeed;                // Speed of moving platforms

    bool isSolid;                   // True if player can stand on it.
    float disappearTimer;           // Timer for disappearing platforms

    // Constructor
    Platform(float platformWidth, float platformHeight, float gameWidth, float gameHeight, int platformIndex, int platformCount);

    // Method to update platform state (e.g., movement, disappearing)
    void update(float gameWidth);

    // Method to handle scrolling and resetting platform
    void scrollAndReset(float playerVelocityY, float gameWidth, float gameHeight, float platformWidth);

    // Method to draw the platform
    void draw(sf::RenderWindow& window) const;

    // Method to get the global bounds of the platform for collision detection
    sf::FloatRect getGlobalBounds() const;

    // Method to reset the platform's state for reappearance
    void resetForReappearance(float gameWidth, float platformWidth);

private:
    // Helper function to set platform properties based on type
    void setPlatformProperties(int typeRoll);
};
