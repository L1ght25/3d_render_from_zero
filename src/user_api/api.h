#pragma once
#include "../camera/Camera.h"
#include "../geometric_primitives/4d_primitives.h"
#include "../rendering/RenderContext.h"
#include "../rendering/bitmap/bitmap.h"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <exception>
#include <iostream>
#include <random>
#include <string_view>
#include <unistd.h>

namespace api {
class View {
  public:
    using Matrix4d = geometry::Matrix4d;

    View(int width = constants::DEFAULT_WIDTH, int height = constants::DEFAULT_HEIGHT);

    void Execute();

    void LoadModel(std::string_view model_path, std::string_view texture_path, bool automode = false, bool is_inversed_z = false, double delta_x = 0,
                   double delta_y = 0, double delta_z = 0);

  private:
    geometry::Matrix4d get_object_transform(double rotate_state);

    geometry::Matrix4d get_object_transform(double rotate_state_x, double rotate_state_y);

    void image_to_bitmap(rendering::Bitmap& my_texture, const sf::Image& text);

    void render(double rotate_state);

    sf::RenderWindow main_window_;
    sf::Texture screen_;

    rendering::RenderContext main_renderer_;

    std::vector<rendering::Bitmap> texures_;
    std::vector<load::Object3d> models_;

    Camera camera_;
};
}  // namespace api
