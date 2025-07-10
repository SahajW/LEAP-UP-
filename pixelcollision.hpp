#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace sf;
bool PerfectPixelCollision(const Sprite& sprite1, const Image& image1,   const Sprite& sprite2, const Image& image2,
                           float intersectionLeft, float intersectionTop, float intersectionWidth, float intersectionHeight)

{
    IntRect texRect1 = sprite1.getTextureRect();  
    IntRect texRect2 = sprite2.getTextureRect();

    for (int i = static_cast<int>(intersectionLeft); i < static_cast<int>(intersectionLeft + intersectionWidth); ++i)
    {
        for (int j = static_cast<int>(intersectionTop); j < static_cast<int>(intersectionTop + intersectionHeight); ++j)
        {
           Vector2f local1 = sprite1.getInverseTransform().transformPoint({static_cast<float>(i), static_cast<float>(j)});
           Vector2f local2 = sprite2.getInverseTransform().transformPoint({static_cast<float>(i), static_cast<float>(j)});

              if (local1.x >= 0 && local1.y >= 0 &&
                local1.x < texRect1.size.x && local1.y < texRect1.size.y &&
                local2.x >= 0 && local2.y >= 0 &&
                local2.x < texRect2.size.x && local2.y < texRect2.size.y)
                {
                      Color c1 = image1.getPixel({static_cast<unsigned>(local1.x) + texRect1.position.x,
                                               static_cast<unsigned>(local1.y) + texRect1.position.y});
                      Color c2 = image2.getPixel({static_cast<unsigned>(local2.x) + texRect2.position.x,
                                               static_cast<unsigned>(local2.y) + texRect2.position.y});
                            
                        if (c1.a > 0 && c2.a > 0) // Check if both pixels are not transparent
                        {
                            return true; // Collision detected
                        }
                }
        }
    }
    return false; // No collision detected
}