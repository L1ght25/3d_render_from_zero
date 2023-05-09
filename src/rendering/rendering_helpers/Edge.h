#pragma once
#include "Gradients.h"

namespace rendering {
class Edge {
public:
using Vertex = geometry::Vertex;
using Vector4d = geometry::Vector4d;

    Edge(const Gradients& grad, const Vertex& first, const Vertex& second, int ind_of_min_v_color);

    void Step();

    const Vector4d& GetTexturePos() const;

    double GetX() const;

    int GetYStart() const;

    int GetYEnd() const;

    double GetDepth() const;

    double GetOneOverZ() const;

private:
    int y_start_;
    int y_end_;
    double x_step_;
    double cur_x_;

    Vector4d cur_texture_pos_;
    Vector4d texture_pos_step_;

    double cur_depth_;
    double depth_step_;

    double cur_one_over_z_;
    double cur_z_step_;
};
}