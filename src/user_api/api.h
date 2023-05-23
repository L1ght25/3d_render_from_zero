#pragma once
#include "../camera/Camera.h"
#include "../geometric_primitives/4d_primitives.h"
#include "../rendering/RenderContext.h"
#include "../rendering/bitmap/bitmap.h"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <exception>
#include <initializer_list>
#include <iostream>
#include <random>
#include <string_view>
#include <unistd.h>
#include <ranges>


namespace api {
class Application {
  public:
    using Matrix4d = geometry::Matrix4d;
    using Object3d = load::Object3d;

    Application(std::initializer_list<Object3d>models, int width = constants::default_width, int height = constants::default_height);

    void Execute();

  private:

    geometry::Matrix4d get_object_transform(double rotate_state);

    geometry::Matrix4d get_object_transform(double rotate_state_x, double rotate_state_y);

    void render(double rotate_state);

    sf::RenderWindow main_window_;
    sf::Texture screen_;

    rendering::RenderContext main_renderer_;

    std::vector<load::Object3d> models_;

    Camera camera_;
};
}  // namespace api
