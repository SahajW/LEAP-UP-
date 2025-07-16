#include "Platform.h"
#include <iostream> // For debugging, if needed

// Constructor implementation
Platform::Platform(float platformWidth, float platformHeight, float gameWidth, float gameHeight, int platformIndex, int platformCount) {
    shape.setSize(sf::Vector2f(platformWidth, platformHeight));

    // Initialize platform type and properties
    setPlatformProperties(rand() % 100);

    // Set initial position
    float x = rand() % (static_cast<int>(gameWidth - platformWidth)); // Ensure platform stays within gameWidth
    float y = platformIndex * (gameHeight / platformCount); // Evenly spaced vertically
    shape.setPosition({ x, y });
    originalPosition = { x, y }; // Store original position for scrolling
}

// Helper function to set platform properties based on type
void Platform::setPlatformProperties(int typeRoll) {
    if (typeRoll < 70) { // 70% chance for normal platforms
        type = NORMAL;
        shape.setFillColor(sf::Color::Green);
        isSolid = true;
    }
    else if (typeRoll < 75) { // 5% chance for thorn platforms
        type = THORN;
        shape.setFillColor(sf::Color::Red);
        isSolid = true;
    }
    else if (typeRoll < 90) { // 15% chance for moving platforms
        type = MOVING;
        shape.setFillColor(sf::Color::Blue);
        moveDirection = (rand() % 2 == 0) ? 1.f : -1.f; // Random initial direction (right or left)
        moveSpeed = (rand() % 2 + 1) * 1.5f; // Speed between 1.5 and 3.0
        isSolid = true;
    }
    else { // 10% chance for disappearing platforms
        type = DISAPPEARING;
        shape.setFillColor(sf::Color(128, 0, 128)); // Purple color for disappearing
        isSolid = true; // Starts solid
        disappearTimer = 0.f; // Timer not active initially
    }
}

// Update method for platform specific logic
void Platform::update(float gameWidth) {
    if (type == MOVING) {
        // Move the platform horizontally
        shape.move(sf::Vector2f(moveDirection * moveSpeed, 0));

        // Reverse direction if hitting game world boundaries
        if (shape.getPosition().x <= 0 || shape.getPosition().x + shape.getSize().x >= gameWidth) {
            moveDirection *= -1.f; // Reverse direction
        }
    }
    else if (type == DISAPPEARING) {
        if (disappearTimer > 0) { // If timer is active
            disappearTimer -= 1.f; // Decrement timer (adjust value for desired speed at 60 FPS)
            if (disappearTimer <= 0) { // If timer runs out
                isSolid = false; // Make it non-solid
                shape.setFillColor(sf::Color::Transparent); // Make it invisible
            }
        }
    }
}

// Method to handle scrolling and resetting platform
void Platform::scrollAndReset(float playerVelocityY, float gameWidth, float gameHeight, float platformWidth) {
    // Adjust original position for scrolling
    originalPosition.y += -playerVelocityY;

    // If platform goes off screen bottom, reset to top with new properties
    if (originalPosition.y > gameHeight) {
        resetForReappearance(gameWidth, platformWidth);
    }

    // Update the shape's position based on its original position (for scrolling)
    // and its current horizontal offset if it's a moving platform.
    float currentOffsetX = shape.getPosition().x - originalPosition.x;
    shape.setPosition(sf::Vector2f(originalPosition.x + currentOffsetX, originalPosition.y));
}

// Method to draw the platform
void Platform::draw(sf::RenderWindow& window) const {
    window.draw(shape);
}

// Method to get the global bounds of the platform for collision detection
sf::FloatRect Platform::getGlobalBounds() const {
    return shape.getGlobalBounds();
}

// Method to reset the platform's state for reappearance
void Platform::resetForReappearance(float gameWidth, float platformWidth) {
    originalPosition.y = 0; // Reset to top
    originalPosition.x = rand() % (static_cast<int>(gameWidth - platformWidth)); // New random X

    // Re-randomize type for new platform appearing at the top
    setPlatformProperties(rand() % 100);
    // Ensure it's visible and solid if it's a disappearing platform
    if (type == DISAPPEARING) {
        shape.setFillColor(sf::Color(128, 0, 128)); // Reset to purple
        isSolid = true; // Reset to solid
        disappearTimer = 0.f; // Reset timer
    }
}

