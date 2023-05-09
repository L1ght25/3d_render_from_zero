#pragma once
#include <exception>
#include <random>
#include <chrono>
#include <iostream>
#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <string_view>
#include "../rendering/bitmap/bitmap.h"
#include "../rendering/RenderContext.h"
#include "../geometric_primitives/4d_primitives.h"

enum {
    DEFAULT_WIDTH = 1000,
    DEFAULT_HEIGHT = 1000
};

static constexpr double radToDeg(double angleInRadians) {
    return angleInRadians * 180.0 / M_PI;
}

namespace api {
class View {
public:

using Matrix4d = geometry::Matrix4d;

    View(std::string_view model_path, std::string_view texture_path, int width = DEFAULT_WIDTH, int height = DEFAULT_HEIGHT);

    void Execute();

private:

    geometry::Matrix4d get_transform(double rotate_state);

    void image_to_bitmap(rendering::Bitmap& my_texture, const sf::Image& text);

    void render(double rotate_state);

    sf::RenderWindow main_window_;
    sf::Texture screen_;

    rendering::Bitmap texture_;
    rendering::RenderContext main_renderer_;

    load::Object3d model_;
    geometry::Matrix4d projection_translation_;
};
}
