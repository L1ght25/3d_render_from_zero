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
    using Object3d = model::Object3d;
    using Image = const sf::Uint8*;

    Application(std::initializer_list<Object3d> models, int width = constants::default_width, int height = constants::default_height);

    void Execute();

  private:
    struct SfmlDrawConnection {
      SfmlDrawConnection(int width, int height);

      void Draw(Image image);

      sf::RenderWindow main_window_;
      sf::Texture screen_;
      sf::Sprite sprite_;
    };

    CameraEvent HandleCameraEventsOrClose();

    void UpdateWorld(double& rotate_state, CameraEvent event);

    SfmlDrawConnection sfml_details_;
    rendering::RenderContext main_renderer_;

    std::vector<Object3d> models_;

    Camera camera_;
};
}  // namespace api
