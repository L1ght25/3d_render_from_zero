#include "RenderContext.h"

rendering::RenderContext::RenderContext(int width, int height) : Bitmap(width, height), z_buffer_(width * height) {
}

void rendering::RenderContext::DrawModel(const Object3d& model, const Matrix4d& transform, const Bitmap& texture) {
    for (int i = 0; i < model.SizeOfPolygons(); i += 3) {
        FillTriangle(model.GetVertexByInd(i).Transform(transform), model.GetVertexByInd(i + 1).Transform(transform),
                     model.GetVertexByInd(i + 2).Transform(transform), texture);
    }
}

void rendering::RenderContext::FillTriangle(const Vertex& first_dot, const Vertex& second_dot, const Vertex& third_dot, const Bitmap& texture) {
    static Matrix4d perspective = Matrix4d().InitScreenSpaceTransform((double)height_ / 2, (double)width_ / 2);
    auto first = first_dot.Transform(perspective).PerspectiveDivision();
    auto second = second_dot.Transform(perspective).PerspectiveDivision();
    auto third = third_dot.Transform(perspective).PerspectiveDivision();
    std::vector<Vertex> sorted_dots = {first, second, third};
    std::sort(sorted_dots.begin(), sorted_dots.end(), [](const Vertex& first, const Vertex& second) { return first.GetY() <= second.GetY(); });
    Gradients grad(sorted_dots[0], sorted_dots[1], sorted_dots[2]);
    TriangleHandler(grad, sorted_dots[0], sorted_dots[1], sorted_dots[2], sorted_dots[0].SquareTriangleTwice(sorted_dots[2], sorted_dots[1]) >= 0,
                    texture);
}

void rendering::RenderContext::ClearZBuffer() {
    for (auto& pixel : z_buffer_) {
        pixel = MAXFLOAT;
    }
}

void rendering::RenderContext::TriangleHandler(const Gradients& grad, const Vertex& min_y_v, const Vertex& middle_y_v, const Vertex& max_y_v,
                                               bool is_positive_square, const Bitmap& texture) {
    Edge max_to_min(grad, min_y_v, max_y_v, 0);
    Edge max_to_mid(grad, min_y_v, middle_y_v, 0);
    Edge mid_to_min(grad, middle_y_v, max_y_v, 1);
    EdgeScan(grad, &max_to_min, &max_to_mid, is_positive_square, texture);
    EdgeScan(grad, &max_to_min, &mid_to_min, is_positive_square, texture);
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

        geometry::Vector4d cur_texture_pos = left.GetTexturePos() + grad.GetXStep() * (std::ceil(left.GetX()) - left.GetX());
        double cur_depth = left.GetDepth() + grad.GetDepthStepX() * (std::ceil(left.GetX()) - left.GetX());
        double cur_one_over_z = left.GetOneOverZ() + grad.GetOneOverZX() * (std::ceil(left.GetX()) - left.GetX());
        double cur_z_step = (right.GetOneOverZ() - left.GetOneOverZ()) / (right.GetX() - left.GetX());

        for (int x = std::ceil(left.GetX()); x < std::ceil(right.GetX()); ++x) {
            int cur_ind = y * width_ + x;
            if (cur_depth < z_buffer_[cur_ind]) {
                z_buffer_[cur_ind] = cur_depth;
                double cur_z = 1.f / cur_one_over_z;
                int tex_x = std::round(cur_texture_pos.GetX() * cur_z * (texture.Width() - 1));
                int tex_y = std::round(cur_texture_pos.GetY() * cur_z * (texture.Height() - 1));
                if (texture.IsInsideMap(tex_x, tex_y)) {  // TODO: нормально отдежабить
                    CopyPixel(texture, x, y, tex_x, tex_y);
                }
            }
            cur_texture_pos += grad.GetXStep();
            cur_depth += grad.GetDepthStepX();
            cur_one_over_z += cur_z_step;
        }
        left.Step();
        right.Step();
    }
}