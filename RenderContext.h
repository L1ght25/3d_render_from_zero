#pragma once
#include <utility>

#include "bitmap.h"
#include "Vector4d.h"

class Vertex {
public:

    Vertex(double x, double y, double z, Vector4d color) : pos_(x, y, z, 1), texture_pos_(std::move(color)) {

    }

    Vertex(Vector4d vec, Vector4d color) : pos_(std::move(vec)), texture_pos_(std::move(color)) {

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
            texture_pos_{min_y_v.GetTexturePos(), mid_y_v.GetTexturePos(), max_y_v.GetTexturePos()} {
        double dX = (mid_y_v.GetX() - max_y_v.GetX()) * (min_y_v.GetY() - max_y_v.GetY()) -
                    (min_y_v.GetX() - max_y_v.GetX()) * (mid_y_v.GetY() - max_y_v.GetY());
        double dY = -dX;
        x_step_ = ((texture_pos_[1] - texture_pos_[2]) * (min_y_v.GetY() - max_y_v.GetY()) -
                  (texture_pos_[0] - texture_pos_[2]) * (mid_y_v.GetY() - max_y_v.GetY())) * (1.f / dX);
        y_step_ = ((texture_pos_[1] - texture_pos_[2]) * (min_y_v.GetX() - max_y_v.GetX()) -
                  (texture_pos_[0] - texture_pos_[2]) * (mid_y_v.GetX() - max_y_v.GetX())) * (1.f / dY);
    }

    const Vector4d& GetColorPos(int pos) const {
        return texture_pos_[pos];
    }

    const Vector4d& GetXStep() const {
        return x_step_;
    }

    Vector4d GetYStep() const {
        return y_step_;
    }

private:
    Vector4d texture_pos_[3];
    Vector4d x_step_;
    Vector4d y_step_;

};

class Edge {
public:
    Edge(const Gradients& grad, const Vertex& first, const Vertex& second, int ind_of_min_v_color) :
            y_start_(std::ceil(first.GetY())), y_end_(std::ceil(second.GetY())),
            x_step_((first.GetX() - second.GetX()) / (first.GetY() - second.GetY())),
            cur_x_(std::ceil(first.GetX()) + x_step_ * (std::ceil(first.GetY()) - first.GetY())),
            cur_texture_pos_(grad.GetColorPos(ind_of_min_v_color) + grad.GetYStep() * (std::ceil(first.GetY()) - first.GetY())
            + grad.GetXStep() * (cur_x_ - first.GetX())),
            texture_pos_step_(grad.GetYStep() + grad.GetXStep() * x_step_){
    }

    void Step() {
        cur_x_ += x_step_;
        cur_texture_pos_ += texture_pos_step_;
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

private:
    int y_start_;
    int y_end_;
    double x_step_;
    double cur_x_;

    Vector4d cur_texture_pos_;
    Vector4d texture_pos_step_;
};

class RenderContext : public Bitmap {
public:
    RenderContext(int width, int height) : Bitmap(width, height) {

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
            for (int x = std::ceil(left.GetX()); x < std::ceil(right.GetX()); ++x) {
//                SetPixel(x, y, 255, 255, 255, 255);
                int tex_x = std::round(cur_texture_pos.GetX() * (texture.Width() - 1));
                int tex_y = std::round(cur_texture_pos.GetY() * (texture.Height() - 1));
                CopyPixel(texture, x, y, tex_x, tex_y);
//                SetPixel(x, y, std::round(cur_texture_pos.GetX() * 255), std::round(cur_texture_pos.GetY() * 255), std::round(cur_texture_pos.GetZ() * 255), 255);
                cur_texture_pos += grad.GetXStep();
            }
            left.Step();
            right.Step();
        }
    }
};