#include "RenderContext.h"
#include "bitmap/bitmap.h"
#include <cstdio>

rendering::RenderContext::RenderContext(int width, int height) : screen_pixels_(width, height), z_buffer_(width * height) {
}

void rendering::RenderContext::ClearZBuffer() {
    for (auto& pixel : z_buffer_) {
        pixel = MAXFLOAT;
    }
}

const sf::Uint8* rendering::RenderContext::MakeImage(const std::vector<Object3d>& models, const Camera camera, double rotate_state) {
    PrepareForDraw();

    auto identity = Matrix4d::InitIdentityOperator();
    auto camera_transform = camera.GetTransform();
    auto spinning_transform = geometry::Matrix4d::InitRotation(0, rotate_state, rotate_state);
    auto spinning_full_transform = camera_transform * spinning_transform;

    for (const auto& model : models) {
        if (model.IsAutoTransformed()) {
            DrawModel(model, spinning_full_transform, spinning_transform);
        } else {
            DrawModel(model, camera_transform, identity);
        }
    }
    return screen_pixels_.GetPointerToPixels();
}

void rendering::RenderContext::PrepareForDraw() {
    ClearZBuffer();
    screen_pixels_.Fill(constants::default_grey);
}

void rendering::RenderContext::DrawModel(const Object3d& model, const Matrix4d& full_transform, const Matrix4d& obj_transform) {
    auto texture = model.GetTexture();
    for (int i = 0; i < model.SizeOfPolygons(); i += 3) {
        DrawTriangle(model.GetVertexByInd(i).TransformBy(full_transform, obj_transform),
                     model.GetVertexByInd(i + 1).TransformBy(full_transform, obj_transform),
                     model.GetVertexByInd(i + 2).TransformBy(full_transform, obj_transform), texture);
    }
}

void rendering::RenderContext::DrawTriangle(const Vertex& first_dot, const Vertex& second_dot, const Vertex& third_dot, const Bitmap& texture) {

    bool first_is_inside = first_dot.IsInsideCubedView();
    bool second_is_inside = second_dot.IsInsideCubedView();
    bool third_is_inside = third_dot.IsInsideCubedView();

    if (first_is_inside && second_is_inside && third_is_inside) {
        FillTriangle(first_dot, second_dot, third_dot, texture);
        return;
    } else if (!first_is_inside && !second_is_inside && !third_is_inside) {
        return;
    }
    std::vector<Vertex> vertices{first_dot, second_dot, third_dot};
    std::vector<Vertex> tmp_vertices;

    if (ClipTriangleAlongAxis(vertices, tmp_vertices, 0) && ClipTriangleAlongAxis(vertices, tmp_vertices, 1) &&
        ClipTriangleAlongAxis(vertices, tmp_vertices, 2)) {
        auto first_clipped_dot = vertices[0];
        for (int i = 1; i < vertices.size() - 1; ++i) {
            FillTriangle(first_clipped_dot, vertices[i], vertices[i + 1], texture);
        }
    }
}

void rendering::RenderContext::ClipTriangleAlongComponent(std::vector<Vertex>& vertices, std::vector<Vertex>& clipped_vertices, int comp,
                                                          int factor) {
    const Vertex* prev = &vertices.back();
    bool prev_is_inside = prev->GetPos()[comp] * factor <= prev->GetPos().GetW();

    for (auto&& vertex : vertices) {

        bool curr_is_inside = vertex.GetPos()[comp] * factor <= vertex.GetPos().GetW();

        if (curr_is_inside ^ prev_is_inside) {
            double coef = (prev->GetW() - prev->GetPos()[comp] * factor) /
                          ((prev->GetW() - prev->GetPos()[comp] * factor) - (vertex.GetW() - vertex.GetPos()[comp] * factor));
            clipped_vertices.emplace_back(prev->GetLinearInterpolationOfVertices(vertex, coef));
        }
        if (curr_is_inside) {
            clipped_vertices.emplace_back(std::move(vertex));
        }

        prev = curr_is_inside ? &clipped_vertices.back() : &vertex;
        prev_is_inside = curr_is_inside;
    }

    vertices.clear();
}

bool rendering::RenderContext::ClipTriangleAlongAxis(std::vector<Vertex>& vertices, std::vector<Vertex>& clipped_vertices, int comp) {
    ClipTriangleAlongComponent(vertices, clipped_vertices, comp, 1);

    if (clipped_vertices.empty()) {
        return false;
    }
    ClipTriangleAlongComponent(clipped_vertices, vertices, comp, -1);

    return !vertices.empty();
}

void rendering::RenderContext::TriangleHandler(const Gradients& grad, const Vertex& min_y_v, const Vertex& middle_y_v, const Vertex& max_y_v,
                                               bool is_positive_square, const Bitmap& texture) {
    Edge max_to_min(grad, min_y_v, max_y_v, 0);
    Edge max_to_mid(grad, min_y_v, middle_y_v, 0);
    Edge mid_to_min(grad, middle_y_v, max_y_v, 1);
    EdgeScan(grad, &max_to_min, &max_to_mid, is_positive_square, texture);
    EdgeScan(grad, &max_to_min, &mid_to_min, is_positive_square, texture);
}

void rendering::RenderContext::FillTriangle(const Vertex& first_dot, const Vertex& second_dot, const Vertex& third_dot, const Bitmap& texture) {
    static auto perspective = Matrix4d::InitScreenSpaceTransform((double)screen_pixels_.Height() / 2, (double)screen_pixels_.Width() / 2);
    auto first = first_dot.TransformBy(perspective).PerspectiveDivision();
    auto second = second_dot.TransformBy(perspective).PerspectiveDivision();
    auto third = third_dot.TransformBy(perspective).PerspectiveDivision();
    std::vector<Vertex> sorted_dots = {first, second, third};
    std::sort(sorted_dots.begin(), sorted_dots.end(), [](const Vertex& first, const Vertex& second) { return first.GetY() <= second.GetY(); });
    Gradients grad(sorted_dots[0], sorted_dots[1], sorted_dots[2]);
    TriangleHandler(grad, sorted_dots[0], sorted_dots[1], sorted_dots[2], sorted_dots[0].IsPositiveSquare(sorted_dots[2], sorted_dots[1]),
                    texture);
}

void rendering::RenderContext::EdgeScan(const Gradients& grad, Edge* first, Edge* second, bool is_positive_square, const Bitmap& texture) {
    Edge* left = first;
    Edge* right = second;
    if (is_positive_square) {
        std::swap(left, right);
    }
    DrawLeftRight(grad, *left, *right, second->GetYStart(), second->GetYEnd(), texture);
}

void rendering::RenderContext::DrawLeftRight(const Gradients& grad, Edge& left, Edge& right, int y_min, int y_max, const Bitmap& texture) {
    for (int y = y_min; y < y_max; ++y) {

        double x_prestep = std::ceil(left.GetX()) - left.GetX();

        geometry::Vector4d cur_texture_pos = left.GetTexturePos() + grad.GetXStep() * x_prestep;
        double cur_depth = left.GetDepth() + grad.GetDepthStepX() * x_prestep;
        double cur_one_over_z = left.GetOneOverZ() + grad.GetOneOverZX() * x_prestep;
        double cur_light = left.GetLight() + grad.GetLightXStep() * x_prestep;

        for (int x = int(std::ceil(left.GetX())); x < int(std::ceil(right.GetX())); ++x) {
            int cur_ind = y * screen_pixels_.Width() + x;
            if (cur_depth < z_buffer_[cur_ind]) {
                z_buffer_[cur_ind] = cur_depth;
                double cur_z = 1.f / cur_one_over_z;
                int tex_x = std::round(cur_texture_pos.GetX() * cur_z * (texture.Width() - 1));
                int tex_y = std::round(cur_texture_pos.GetY() * cur_z * (texture.Height() - 1));
                int light = std::round(cur_light * constants::max_light);
                if (texture.IsInsideMap(tex_x, tex_y)) {
                    screen_pixels_.CopyPixel(texture, x, y, tex_x, tex_y, cur_light);
                }
            }
            cur_texture_pos += grad.GetXStep();
            cur_depth += grad.GetDepthStepX();
            cur_one_over_z += grad.GetOneOverZX();
            cur_light += grad.GetLightXStep();
        }
        left.Step();
        right.Step();
    }
}
