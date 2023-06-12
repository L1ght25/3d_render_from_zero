#pragma once
#include <fstream>
#include <limits>
#include <sstream>
#include <string_view>
#include <utility>
#include <vector>

#include "../constants/constants.h"
#include "../geometric_primitives/4d_primitives.h"
#include "../geometric_primitives/Vertex.h"
#include "../load_models/Object3d.h"
#include "bitmap/bitmap.h"
#include "rendering_helpers/Edge.h"
#include "rendering_helpers/Gradients.h"
#include "../camera/Camera.h"

namespace rendering {
class RenderContext {
  public:
    using Vertex = geometry::Vertex;
    using Matrix4d = geometry::Matrix4d;
    using Gradients = rendering::Gradients;
    using Edge = rendering::Edge;
    using Object3d = model::Object3d;
    using Camera = api::Camera;

    RenderContext(int width, int height);

    const sf::Uint8* MakeImage(const std::vector<Object3d>& models, const Camera camera, double rotate_state);

  private:
    void ClearZBuffer();
    void PrepareForDraw();
    void DrawModel(const Object3d& model, const Matrix4d& full_transform, const Matrix4d& obj_transform);
    void DrawTriangle(const Vertex& first_dot, const Vertex& second_dot, const Vertex& third_dot, const Bitmap& texture);

    bool ClipTriangleAlongAxis(std::vector<Vertex>& vertices, std::vector<Vertex>& clipped_vertices, int comp);
    void ClipTriangleAlongComponent(std::vector<Vertex>& vertices, std::vector<Vertex>& clipped_vertices, int comp, int factor);

    void TriangleHandler(const Gradients& grad, const Vertex& min_y_v, const Vertex& middle_y_v, const Vertex& max_y_v, bool is_positive_square,
                         const Bitmap& texture);
    void FillTriangle(const Vertex& first_dot, const Vertex& second_dot, const Vertex& third_dot, const Bitmap& texture);

    void EdgeScan(const Gradients& grad, Edge* first, Edge* second, bool is_positive_square, const Bitmap& texture);

    void DrawLeftRight(const Gradients& grad, Edge& left, Edge& right, int y_min, int y_max, const Bitmap& texture);

    std::vector<double> z_buffer_;
    Bitmap screen_pixels_;
};
}  // namespace rendering
