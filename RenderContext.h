#pragma once
#include <utility>
#include <vector>
#include <string_view>
#include <fstream>
#include <sstream>
#include <limits>

#include "bitmap.h"
#include "Vector4d.h"

class Object3d;

class Vertex {
public:
    Vertex(double x, double y, double z) : pos_(x, y, z, 1), texture_pos_(0, 0, 0, 0) {

    }

    Vertex(double x, double y, double z, Vector4d color) : pos_(x, y, z, 1), texture_pos_(std::move(color)) {

    }

    Vertex(Vector4d vec, Vector4d color) : pos_(std::move(vec)), texture_pos_(std::move(color)) {

    }

    void SetTexturePos(Vector4d pos) {
        texture_pos_ = std::move(pos);
    }

    double GetX() const {
        return pos_.GetX();
    }

    double GetY() const {
        return pos_.GetY();
    }

    double GetZ() const {
        return pos_.GetZ();
    }

    const Vector4d& GetPos() const {
        return pos_;
    }

    Vector4d GetTexturePos() const {
    return texture_pos_;
    }

    Vertex Transform(const Matrix4d& oper) const {
        return {pos_.Transform(oper), texture_pos_};
    }

    Vertex PerspectiveDivision() {
        return {Vector4d{pos_.GetX() / pos_.GetW(), pos_.GetY() / pos_.GetW(), pos_.GetZ() / pos_.GetW(), pos_.GetW()}, texture_pos_};
    }

    double SquareTriangleTwice(const Vertex& second_v, const Vertex& third_v) const {
        return (second_v.GetX() - GetX()) * (third_v.GetY() - GetY()) - (third_v.GetX() - GetX()) * (second_v.GetY() - GetY());
    }

private:
    Vector4d pos_;
    Vector4d texture_pos_;
};

class Gradients {
public:
    Gradients(const Vertex& min_y_v, const Vertex& mid_y_v, const Vertex& max_y_v) :
            texture_pos_{min_y_v.GetTexturePos(), mid_y_v.GetTexturePos(), max_y_v.GetTexturePos()},
            depth_(min_y_v.GetZ(), mid_y_v.GetZ(), max_y_v.GetZ(), 0) {
        double dX = (mid_y_v.GetX() - max_y_v.GetX()) * (min_y_v.GetY() - max_y_v.GetY()) -
                    (min_y_v.GetX() - max_y_v.GetX()) * (mid_y_v.GetY() - max_y_v.GetY());
        double dY = -dX;
        x_step_ = calc_step(texture_pos_, min_y_v, mid_y_v, max_y_v, dX, true);
        y_step_ = calc_step(texture_pos_, min_y_v, mid_y_v, max_y_v, dY, false);
        depth_step_(0, 0) = ((depth_(1, 0) - depth_(2, 0)) * (min_y_v.GetY() - max_y_v.GetY()) -
                             (depth_(0, 0) - depth_(2, 0)) * (mid_y_v.GetY() - max_y_v.GetY())) * (1.f / dX);
        depth_step_(1, 0) = ((depth_(1, 0) - depth_(2, 0)) * (min_y_v.GetX() - max_y_v.GetX()) -
                             (depth_(0, 0) - depth_(2, 0)) * (mid_y_v.GetX() - max_y_v.GetX())) * (1.f / dY);
    }

    const Vector4d& GetTexPos(int pos) const {
        return texture_pos_[pos];
    }

    double GetDepthPos(int pos) const {
        return depth_(pos, 0);
    }

    double GetDepthStepX() const {
        return depth_step_(0, 0);
    }

    double GetDepthStepY() const {
        return depth_step_(1, 0);
    }

    const Vector4d& GetXStep() const {
        return x_step_;
    }

    Vector4d GetYStep() const {
        return y_step_;
    }

private:

    Vector4d calc_step(Vector4d* vec, const Vertex& min_y_v, const Vertex& mid_y_v, const Vertex& max_y_v,
                    double diff, bool is_x) {
        auto tmp = is_x ? ((vec[1] - vec[2]) * (min_y_v.GetY() - max_y_v.GetY()) -
                           (vec[0] - vec[2]) * (mid_y_v.GetY() - max_y_v.GetY())) :
                                   ((vec[1] - vec[2]) * (min_y_v.GetX() - max_y_v.GetX()) -
                                   (vec[0] - vec[2]) * (mid_y_v.GetX() - max_y_v.GetX()));
        return tmp * (1.f / diff);
    }

    Vector4d texture_pos_[3];
    Vector4d x_step_;
    Vector4d y_step_;
    Vector4d depth_;
    Vector4d depth_step_;
};

class Edge {
public:
    Edge(const Gradients& grad, const Vertex& first, const Vertex& second, int ind_of_min_v_color) :
            y_start_(std::ceil(first.GetY())), y_end_(std::ceil(second.GetY())),
            x_step_((first.GetX() - second.GetX()) / (first.GetY() - second.GetY())),
            cur_x_(std::ceil(first.GetX()) + x_step_ * (std::ceil(first.GetY()) - first.GetY())),
            cur_texture_pos_(grad.GetTexPos(ind_of_min_v_color) + grad.GetYStep() * (std::ceil(first.GetY()) - first.GetY())
            + grad.GetXStep() * (cur_x_ - first.GetX())),
            texture_pos_step_(grad.GetYStep() + grad.GetXStep() * x_step_),
            cur_depth_(grad.GetDepthPos(ind_of_min_v_color) + grad.GetDepthStepY() *
            (std::ceil(first.GetY()) - first.GetY()) + grad.GetDepthStepX() * (cur_x_ - first.GetX())),
            depth_step_(grad.GetDepthStepY() + grad.GetDepthStepX() * x_step_) {
    }

    void Step() {
        cur_x_ += x_step_;
        cur_texture_pos_ += texture_pos_step_;
        cur_depth_ += depth_step_;
    }

    const Vector4d& GetTexturePos() const {
        return cur_texture_pos_;
    }

    double GetX() const {
        return cur_x_;
    }

    int GetYStart() const {
        return y_start_;
    }

    int GetYEnd() const {
        return y_end_;
    }

    double GetDepth() const {
        return cur_depth_;
    }

private:
    int y_start_;
    int y_end_;
    double x_step_;
    double cur_x_;

    Vector4d cur_texture_pos_;
    Vector4d texture_pos_step_;

    double cur_depth_;
    double depth_step_;
};

class Object3d {
public:
    Object3d(std::string_view filename) {
        std::vector<Vector4d> texture_coords_;
        std::ifstream in(filename, std::ios::in);
        if (!in) {
            throw std::runtime_error("Cannot read file");
        }
        std::string curr_line;

        while (getline(in, curr_line)) {
            if (curr_line.starts_with("v ") or curr_line.starts_with("vt ")) {
                std::istringstream in_vert(curr_line.substr(2));
                double x, y, z;
                in_vert >> x;
                in_vert >> y;
                if (curr_line.starts_with("v ")) {
                    in_vert >> z;
                    vertices_.emplace_back(x, y, z);
                }
                else {
                    texture_coords_.emplace_back(x, y, 0, 0);
                }
            } else if(curr_line.starts_with("f ")) {
                int a, b, c;  // vertices
                int A, B, C;  // texture
                int AA, BB, CC; // ignore for good times
                const char *curr_str = curr_line.c_str();
                sscanf(curr_str, "f %i/%i/%i %i/%i/%i %i/%i/%i", &a, &A, &AA, &b, &B, &BB, &c, &C, &CC);
                --a, --b, --c, --A, --B, --C;
                indexes_.push_back(a);
                indexes_.push_back(b);
                indexes_.push_back(c);
                vertices_[a].SetTexturePos(texture_coords_[A]);
                vertices_[b].SetTexturePos(texture_coords_[B]);
                vertices_[c].SetTexturePos(texture_coords_[C]);
            }
        }
    }

    const Vertex& GetVertexByInd(int i) const {
        return vertices_[indexes_[i]];
    }

    size_t SizeOfPolygons() const {
        return indexes_.size();
    }

private:
    std::vector<Vertex> vertices_;
    std::vector<size_t>indexes_;
};

class RenderContext : public Bitmap {
public:
    RenderContext(int width, int height) : Bitmap(width, height), z_buffer_(width * height) {

    }

    void DrawModel(const Object3d& model, const Matrix4d& transform, const Bitmap& texture) {
        for (int i = 0; i < model.SizeOfPolygons(); i += 3) {
            FillTriangle(model.GetVertexByInd(i).Transform(transform),
                         model.GetVertexByInd(i + 1).Transform(transform),
                         model.GetVertexByInd(i + 2).Transform(transform), texture);
        }
    }

    void FillTriangle(const Vertex& first_dot, const Vertex& second_dot, const Vertex& third_dot, const Bitmap& texture) {
        Matrix4d perspective = Matrix4d().InitScreenSpaceTransform((double)height_ / 2, (double)width_ / 2);
        auto first = first_dot.Transform(perspective).PerspectiveDivision();
        auto second = second_dot.Transform(perspective).PerspectiveDivision();
        auto third = third_dot.Transform(perspective).PerspectiveDivision();
        std::vector<Vertex> sorted_dots = {first, second, third};
        std::sort(sorted_dots.begin(), sorted_dots.end(),
                  [](const Vertex& first, const Vertex& second) { return first.GetY() <= second.GetY(); });
        Gradients grad(sorted_dots[0], sorted_dots[1], sorted_dots[2]);
        TriangleHandler(grad, sorted_dots[0], sorted_dots[1], sorted_dots[2],
                        sorted_dots[0].SquareTriangleTwice(sorted_dots[2], sorted_dots[1]) >= 0, texture);
    }

    void ClearZBuffer() {
        for (auto& pixel : z_buffer_) {
            pixel = MAXFLOAT;
        }
    }

private:

    void TriangleHandler(const Gradients& grad, const Vertex& min_y_v, const Vertex& middle_y_v, const Vertex& max_y_v,
                         bool is_positive_square, const Bitmap& texture) {
        Edge max_to_min(grad, min_y_v, max_y_v, 0);
        Edge max_to_mid(grad, min_y_v, middle_y_v, 0);
        Edge mid_to_min(grad, middle_y_v, max_y_v, 1);
        EdgeScan(grad, &max_to_min, &max_to_mid, is_positive_square, texture);
        EdgeScan(grad,&max_to_min, &mid_to_min, is_positive_square, texture);
    }

    void EdgeScan(const Gradients& grad, Edge* first, Edge* second, bool is_positive_square, const Bitmap& texture) {
        Edge* left = first;
        Edge* right = second;
        if (is_positive_square) {
            std::swap(left, right);
        }
        DrawLeftRight(grad, *left, *right, second->GetYStart(), second->GetYEnd(), texture);
    }

    void DrawLeftRight(const Gradients& grad, Edge& left, Edge& right, int y_min, int y_max, const Bitmap& texture) {
        for (int y = y_min; y < y_max; ++y) {
            Vector4d cur_texture_pos = left.GetTexturePos() + grad.GetXStep() * (std::ceil(left.GetX()) - left.GetX());

//            double depth_step = (right.GetDepth() - left.GetDepth()) / (std::ceil(right.GetX()) - std::ceil(left.GetX()));
            double cur_depth = left.GetDepth() + grad.GetDepthStepX() * (std::ceil(left.GetX()) - left.GetX());
            for (int x = std::ceil(left.GetX()); x < std::ceil(right.GetX()); ++x) {
                int cur_ind = y * width_ + x;
                if (cur_depth < z_buffer_[cur_ind]) {
                    z_buffer_[cur_ind] = cur_depth;
                    int tex_x = std::round(cur_texture_pos.GetX() * (texture.Width() - 1));
                    int tex_y = std::round(cur_texture_pos.GetY() * (texture.Height() - 1));
                    CopyPixel(texture, x, y, tex_x, tex_y);
                }
                cur_texture_pos += grad.GetXStep();
                cur_depth += grad.GetDepthStepX();
            }
            left.Step();
            right.Step();
        }
    }

    std::vector<double> z_buffer_;
};