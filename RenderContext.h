#pragma once
#include <utility>

#include "bitmap.h"
#include "Vector4d.h"

class Vertex {
public:

    Vertex(double x, double y, double z, Vector4d color) : pos_(x, y, z, 1), color_(std::move(color)) {

    }

    Vertex(Vector4d vec, Vector4d color) : pos_(std::move(vec)), color_(std::move(color)) {

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

    Vector4d GetColor() const {
    return color_;
    }

    Vertex Transform(const Matrix4d& oper) const {
        return {pos_.Transform(oper), color_};
    }

    Vertex PerspectiveDivision() {
        return {Vector4d{pos_.GetX() / pos_.GetW(), pos_.GetY() / pos_.GetW(), pos_.GetZ() / pos_.GetW(), pos_.GetW()}, color_};
    }

    double SquareTriangleTwice(const Vertex& second_v, const Vertex& third_v) const {
        return (second_v.GetX() - GetX()) * (third_v.GetY() - GetY()) - (third_v.GetX() - GetX()) * (second_v.GetY() - GetY());
    }

private:
    Vector4d pos_;
    Vector4d color_;
};

class Gradients {
public:
    Gradients(const Vertex& min_y_v, const Vertex& mid_y_v, const Vertex& max_y_v) :
            color_{min_y_v.GetColor(), mid_y_v.GetColor(), max_y_v.GetColor()} {
        double dX = (mid_y_v.GetX() - max_y_v.GetX()) * (min_y_v.GetY() - max_y_v.GetY()) -
                    (min_y_v.GetX() - max_y_v.GetX()) * (mid_y_v.GetY() - max_y_v.GetY());
        double dY = -dX;
        x_step_ = ((color_[1] - color_[2]) * (min_y_v.GetY() - max_y_v.GetY()) -
                  (color_[0] - color_[2]) * (mid_y_v.GetY() - max_y_v.GetY())) * (1.f / dX);
        y_step_ = ((color_[1] - color_[2]) * (min_y_v.GetX() - max_y_v.GetX()) -
                  (color_[0] - color_[2]) * (mid_y_v.GetX() - max_y_v.GetX())) * (1.f / dY);
    }

    const Vector4d& GetColorPos(int pos) const {
        return color_[pos];
    }

    const Vector4d& GetXStep() const {
        return x_step_;
    }

    Vector4d GetYStep() const {
        return y_step_;
    }

private:
    Vector4d color_[3];
    Vector4d x_step_;
    Vector4d y_step_;

};

class Edge {
public:
    Edge(const Gradients& grad, const Vertex& first, const Vertex& second, int ind_of_min_v_color) :
            y_start_(std::ceil(first.GetY())), y_end_(std::ceil(second.GetY())),
            x_step_((first.GetX() - second.GetX()) / (first.GetY() - second.GetY())),
            cur_x_(std::ceil(first.GetX()) + x_step_ * (std::ceil(first.GetY()) - first.GetY())),
            cur_color_(grad.GetColorPos(ind_of_min_v_color) + grad.GetYStep() * (std::ceil(first.GetY()) - first.GetY())
            + grad.GetXStep() * (cur_x_ - first.GetX())),
            color_step_(grad.GetYStep() + grad.GetXStep() * x_step_){
    }

    void Step() {
        cur_x_ += x_step_;
        cur_color_ += color_step_;
    }

    const Vector4d& GetColor() const {
        return cur_color_;
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

    Vector4d cur_color_;
    Vector4d color_step_;
};

class RenderContext : public Bitmap {
public:
    RenderContext(int width, int height) : Bitmap(width, height) {

    }

    void FillTriangle(const Vertex& first_dot, const Vertex& second_dot, const Vertex& third_dot) {
        Matrix4d perspective = Matrix4d().InitScreenSpaceTransform((double)height_ / 2, (double)width_ / 2);
        auto first = first_dot.Transform(perspective).PerspectiveDivision();
        auto second = second_dot.Transform(perspective).PerspectiveDivision();
        auto third = third_dot.Transform(perspective).PerspectiveDivision();
        std::vector<Vertex> sorted_dots = {first, second, third};
        std::sort(sorted_dots.begin(), sorted_dots.end(),
                  [](const Vertex& first, const Vertex& second) { return first.GetY() <= second.GetY(); });
        Gradients grad(sorted_dots[0], sorted_dots[1], sorted_dots[2]);
        TriangleHandler(grad, sorted_dots[0], sorted_dots[1], sorted_dots[2],
                        sorted_dots[0].SquareTriangleTwice(sorted_dots[2], sorted_dots[1]) >= 0);
    }

private:

    void TriangleHandler(const Gradients& grad, const Vertex& min_y_v, const Vertex& middle_y_v, const Vertex& max_y_v, bool is_positive_square) {
        Edge max_to_min(grad, min_y_v, max_y_v, 0);
        Edge max_to_mid(grad, min_y_v, middle_y_v, 0);
        Edge mid_to_min(grad, middle_y_v, max_y_v, 1);
        EdgeScan(grad, &max_to_min, &max_to_mid, is_positive_square);
        EdgeScan(grad,&max_to_min, &mid_to_min, is_positive_square);
    }

    void EdgeScan(const Gradients& grad, Edge* first, Edge* second, bool is_positive_square) {
        Edge* left = first;
        Edge* right = second;
        if (is_positive_square) {
            std::swap(left, right);
        }
        DrawLeftRight(grad, *left, *right, second->GetYStart(), second->GetYEnd());
    }

    void DrawLeftRight(const Gradients& grad, Edge& left, Edge& right, int y_min, int y_max) {
        for (int y = y_min; y < y_max; ++y) {
            Vector4d cur_color = left.GetColor() + grad.GetXStep() * (std::ceil(left.GetX()) - left.GetX());
            for (int x = std::ceil(left.GetX()); x < std::ceil(right.GetX()); ++x) {
//                SetPixel(x, y, 255, 255, 255, 255);
                SetPixel(x, y, std::round(cur_color.GetX() * 255), std::round(cur_color.GetY() * 255), std::round(cur_color.GetZ() * 255), 255);
                cur_color += grad.GetXStep();
            }
            left.Step();
            right.Step();
        }
    }
};