#pragma once
#include <utility>

#include "bitmap.h"
#include "Vector4d.h"

class Vertex {
public:
    Vertex(double x, double y) : pos_(x, y, 0, 1) {

    }

    Vertex(Vector4d vec) : pos_(std::move(vec)) {

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

    Vertex Transform(const Matrix4d& oper) const {
        return pos_.Transform(oper);
    }

    Vertex PerspectiveDivision() {
        return Vector4d{pos_.GetX() / pos_.GetW(), pos_.GetY() / pos_.GetW(), pos_.GetZ() / pos_.GetW(), pos_.GetW()};
    }

    double SquareTriangleTwice(const Vertex& second_v, const Vertex& third_v) const {
        return (second_v.GetX() - GetX()) * (third_v.GetY() - GetY()) - (third_v.GetX() - GetX()) * (second_v.GetY() - GetY());
    }

private:
    Vector4d pos_;
};

class Edge {
public:
    Edge(const Vertex& first, const Vertex& second) :
            y_start_(std::ceil(first.GetY())), y_end_(std::ceil(second.GetY())),
            x_step_((first.GetX() - second.GetX()) / (first.GetY() - second.GetY())),
            cur_x_(std::ceil(first.GetX()) + x_step_ * (std::ceil(first.GetY()) - first.GetY())) {

    }

    void Step() {
        cur_x_ += x_step_;
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
        TriangleHandler(sorted_dots[0], sorted_dots[1], sorted_dots[2],
                        sorted_dots[0].SquareTriangleTwice(sorted_dots[2], sorted_dots[1]) >= 0);
    }

private:

    void TriangleHandler(const Vertex& min_v, const Vertex& middle_v, const Vertex& max_v, bool is_positive_square) {
        Edge max_to_min(min_v, max_v);
        Edge max_to_mid(min_v, middle_v);
        Edge mid_to_min(middle_v, max_v);
        EdgeScan(&max_to_min, &max_to_mid, is_positive_square);
        EdgeScan(&max_to_min, &mid_to_min, is_positive_square);
    }

    void EdgeScan(Edge* first, Edge* second, bool is_positive_square) {
        Edge* left = first;
        Edge* right = second;
        if (is_positive_square) {
            std::swap(left, right);
        }
        DrawLeftRight(*left, *right, second->GetYStart(), second->GetYEnd());
    }

    void DrawLeftRight(Edge& left, Edge& right, int y_min, int y_max) {
        for (int y = y_min; y < y_max; ++y) {
            for (int x = std::ceil(left.GetX()); x < std::ceil(right.GetX()); ++x) {
                SetPixel(x, y, 255, 255, 255, 255);
            }
            left.Step();
            right.Step();
        }
    }
};