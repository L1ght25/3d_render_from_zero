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
#define WIDTH_TEXTURE 100
#define HEIGHT_TEXTURE 100
#define radToDeg(angleInRadians) ((angleInRadians) * 180.0 / M_PI)

int main() {
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "SFML works!");

    RenderContext my_second_map(WIDTH, HEIGHT);

    Bitmap my_texture(WIDTH_TEXTURE, HEIGHT_TEXTURE);
    for (int x = 0; x < HEIGHT_TEXTURE; ++x) {
        for (int y = 0; y < WIDTH_TEXTURE; ++y) {
            int r = rand() % 256;
            int g = rand() % 256;
            int b = rand() % 256;
            my_texture.SetPixel(x, y, r, g, b, 255);
        }
    }
    Object3d my_first_model("../models/SimpleCube.obj");
//    Vertex first{-1, 1, 0, {1, 1, 0, 0}};
//    Vertex second{0, -1.5, 0, {0, 1, 0, 0}};
//    Vertex third{1, 1, 0, {1, 0, 0, 0}};

    Matrix4d projection = Matrix4d().InitPerspective(radToDeg(double(90)), (double)WIDTH / (HEIGHT), 0.1, 1000.0);
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
        auto rotation = Matrix4d().InitRotation(rotate_state, rotate_state, rotate_state);
        auto transformer = projection_translation * rotation;

        my_second_map.Fill(0);
        my_second_map.ClearZBuffer();
        my_second_map.DrawModel(my_first_model, transformer, my_texture);
//        my_second_map.FillTriangle(first.Transform(transformer), second.Transform(transformer), third.Transform(transformer), my_texture);
        prev_time = curr_time;
        texture.update(my_second_map.GetPointerToPixels());

        window.draw(sprite);
        window.display();
    }
    return 0;
}
