#pragma once
#include "../../constants/constants.h"
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

    double GetLightPos(int pos) const;

    double GetDepthStepX() const;

    double GetDepthStepY() const;

    double GetOneOverZX() const;

    double GetOneOverZY() const;

    const Vector4d& GetXStep() const;

    Vector4d GetYStep() const;

    double GetLightXStep() const;

    double GetLightYStep() const;

    template <class T, class RetType>
    RetType calc_step(T vec, const Vertex& min_y_v, const Vertex& mid_y_v, const Vertex& max_y_v, double diff, bool is_x) {
        auto tmp = is_x ? ((vec[1] - vec[2]) * (min_y_v.GetY() - max_y_v.GetY()) - (vec[0] - vec[2]) * (mid_y_v.GetY() - max_y_v.GetY()))
                        : ((vec[1] - vec[2]) * (min_y_v.GetX() - max_y_v.GetX()) - (vec[0] - vec[2]) * (mid_y_v.GetX() - max_y_v.GetX()));
        return tmp * (1.f / diff);
    }

  private:
    Vector4d texture_pos_[3];
    Vector4d one_over_z_;
    Vector4d depth_;
    Vector4d light_;

    Vector4d x_step_;
    Vector4d y_step_;
    Vector4d depth_step_;
    Vector4d z_step_;
    Vector4d light_step_;
};
}  // namespace rendering