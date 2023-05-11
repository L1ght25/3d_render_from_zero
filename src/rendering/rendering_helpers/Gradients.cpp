#include "Gradients.h"
#include <algorithm>

rendering::Gradients::Gradients(const Vertex& min_y_v, const Vertex& mid_y_v, const Vertex& max_y_v)
    : texture_pos_{min_y_v.GetTexturePos(), mid_y_v.GetTexturePos(), max_y_v.GetTexturePos()},
      depth_(min_y_v.GetZ(), mid_y_v.GetZ(), max_y_v.GetZ(), 0), one_over_z_(1.f / min_y_v.GetW(), 1.f / mid_y_v.GetW(), 1.f / max_y_v.GetW(), 0),
      light_(std::clamp(min_y_v.GetNormal().GetZ(), 0., 1.) * constants::LIGHT_SCALE + (1 - constants::LIGHT_SCALE),
             std::clamp(mid_y_v.GetNormal().GetZ(), 0., 1.) * constants::LIGHT_SCALE + (1 - constants::LIGHT_SCALE),
             std::clamp(max_y_v.GetNormal().GetZ(), 0., 1.) * constants::LIGHT_SCALE + (1 - constants::LIGHT_SCALE), 0) {
    double dX =
        (mid_y_v.GetX() - max_y_v.GetX()) * (min_y_v.GetY() - max_y_v.GetY()) - (min_y_v.GetX() - max_y_v.GetX()) * (mid_y_v.GetY() - max_y_v.GetY());
    double dY = -dX;

    for (size_t i = 0; i < 3; ++i) {
        texture_pos_[i] *= one_over_z_(i, 0);
    }

    z_step_(0, 0) = calc_step<Vector4d, double>(one_over_z_, min_y_v, mid_y_v, max_y_v, dX, true);
    z_step_(1, 0) = calc_step<Vector4d, double>(one_over_z_, min_y_v, mid_y_v, max_y_v, dY, false);

    light_step_(0, 0) = calc_step<Vector4d, double>(light_, min_y_v, mid_y_v, max_y_v, dX, true);
    light_step_(1, 0) = calc_step<Vector4d, double>(light_, min_y_v, mid_y_v, max_y_v, dY, false);

    x_step_ = calc_step<Vector4d*, Vector4d>(texture_pos_, min_y_v, mid_y_v, max_y_v, dX, true);
    y_step_ = calc_step<Vector4d*, Vector4d>(texture_pos_, min_y_v, mid_y_v, max_y_v, dY, false);

    depth_step_(0, 0) = calc_step<Vector4d, double>(depth_, min_y_v, mid_y_v, max_y_v, dX, true);
    depth_step_(1, 0) = calc_step<Vector4d, double>(depth_, min_y_v, mid_y_v, max_y_v, dY, false);
}

const geometry::Vector4d& rendering::Gradients::GetTexPos(int pos) const {
    return texture_pos_[pos];
}

double rendering::Gradients::GetDepthPos(int pos) const {
    return depth_(pos, 0);
}

double rendering::Gradients::GetOneOverZPos(int pos) const {
    return one_over_z_(pos, 0);
}

double rendering::Gradients::GetLightPos(int pos) const {
    return light_(pos, 0);
}

double rendering::Gradients::GetDepthStepX() const {
    return depth_step_(0, 0);
}

double rendering::Gradients::GetDepthStepY() const {
    return depth_step_(1, 0);
}

double rendering::Gradients::GetOneOverZX() const {
    return z_step_(0, 0);
}

double rendering::Gradients::GetOneOverZY() const {
    return z_step_(1, 0);
}

const geometry::Vector4d& rendering::Gradients::GetXStep() const {
    return x_step_;
}

geometry::Vector4d rendering::Gradients::GetYStep() const {
    return y_step_;
}

double rendering::Gradients::GetLightXStep() const {
    return light_step_(0, 0);
}

double rendering::Gradients::GetLightYStep() const {
    return light_step_(1, 0);
}
