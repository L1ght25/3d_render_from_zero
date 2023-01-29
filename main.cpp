#include "bitmap.h"
#include "starfield.h"
#include "RenderContext.h"

#include <random>
#include <chrono>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <unistd.h>

#define WIDTH 1000
#define HEIGHT 1000
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!");

    RenderContext my_second_map(WIDTH, HEIGHT);

    Vertex first{-1, 1};
    Vertex second{0, -1.5};
    Vertex third{1, 1};

    Matrix4d projection = Matrix4d().InitPerspective(radToDeg(double(90)), (double)WIDTH / HEIGHT, 0.1, 1000.0);
    auto translation = Matrix4d().InitTranslationOperator(0, 0, 3);

    auto projection_translation = projection * translation;

    double rotate_state = 0;
    sf::Texture texture;
    texture.create(WIDTH, HEIGHT);
    sf::Sprite sprite(texture);
    texture.update(my_second_map.GetPointerToPixels());
    auto prev_time = std::chrono::system_clock::now();

    while (window.isOpen())
    {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }
        window.clear();
        auto curr_time = std::chrono::system_clock::now();
        std::chrono::duration<double> delta = curr_time - prev_time;
        rotate_state += delta.count();
        auto rotation = Matrix4d().InitRotation(0.5, 1, 0.5, rotate_state);
        auto transformer = projection_translation * rotation;

        my_second_map.Fill(0);
        my_second_map.FillTriangle(first.Transform(transformer), second.Transform(transformer), third.Transform(transformer));
        prev_time = curr_time;
        texture.update(my_second_map.GetPointerToPixels());

        window.draw(sprite);
        window.display();
    }
    return 0;
}
