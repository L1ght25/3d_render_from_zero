#pragma once
#include "../../geometric_primitives/Vertex.h"

namespace rendering {
class Gradients {
  public:
    using Vertex = geometry::Vertex;
    using Vector4d = geometry::Vector4d;

    Gradients(const Vertex& min_y_v, const Vertex& mid_y_v, const Vertex& max_y_v);

    const Vector4d& GetTexPos(int pos) const;

    double GetDepthPos(int pos) const;

    double GetOneOverZPos(int pos) const;

    double GetDepthStepX() const;

    double GetDepthStepY() const;

    double GetOneOverZX() const;

    double GetOneOverZY() const;

    const Vector4d& GetXStep() const;

    Vector4d GetYStep() const;

  private:
    Vector4d calc_step(Vector4d* vec, const Vertex& min_y_v, const Vertex& mid_y_v, const Vertex& max_y_v, double diff, bool is_x);

    Vector4d texture_pos_[3];
    Vector4d x_step_;
    Vector4d y_step_;
    Vector4d depth_;
    Vector4d depth_step_;

    Vector4d one_over_z_;
    Vector4d z_step_;
};
}  // namespace rendering