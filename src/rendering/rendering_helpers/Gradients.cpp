#include "Gradients.h"

rendering::Gradients::Gradients(const Vertex& min_y_v, const Vertex& mid_y_v, const Vertex& max_y_v) :
            texture_pos_{min_y_v.GetTexturePos(), mid_y_v.GetTexturePos(), max_y_v.GetTexturePos()},
            depth_(min_y_v.GetZ(), mid_y_v.GetZ(), max_y_v.GetZ(), 0),
            one_over_z_(1.f / min_y_v.GetW(), 1.f / mid_y_v.GetW(), 1.f / max_y_v.GetW(), 0) {
        double dX = (mid_y_v.GetX() - max_y_v.GetX()) * (min_y_v.GetY() - max_y_v.GetY()) -
                    (min_y_v.GetX() - max_y_v.GetX()) * (mid_y_v.GetY() - max_y_v.GetY());
        double dY = -dX;
        z_step_(0, 0) = ((one_over_z_(1, 0) - one_over_z_(2, 0)) * (min_y_v.GetY() - max_y_v.GetY()) -
                         (one_over_z_(0, 0) - one_over_z_(2, 0)) * (mid_y_v.GetY() - max_y_v.GetY())) * (1.f / dX);

        z_step_(1, 0) = ((one_over_z_(1, 0) - one_over_z_(2, 0)) * (min_y_v.GetX() - max_y_v.GetX()) -
                         (one_over_z_(0, 0) - one_over_z_(2, 0)) * (mid_y_v.GetX() - max_y_v.GetX())) * (1.f / dY);

        for (size_t i = 0; i < 3; ++i) {
                texture_pos_[i] = texture_pos_[i] * one_over_z_(i, 0);
        }
        x_step_ = calc_step(texture_pos_, min_y_v, mid_y_v, max_y_v, dX, true);
        y_step_ = calc_step(texture_pos_, min_y_v, mid_y_v, max_y_v, dY, false);

        depth_step_(0, 0) = ((depth_(1, 0) - depth_(2, 0)) * (min_y_v.GetY() - max_y_v.GetY()) -
                             (depth_(0, 0) - depth_(2, 0)) * (mid_y_v.GetY() - max_y_v.GetY())) * (1.f / dX);
        depth_step_(1, 0) = ((depth_(1, 0) - depth_(2, 0)) * (min_y_v.GetX() - max_y_v.GetX()) -
                             (depth_(0, 0) - depth_(2, 0)) * (mid_y_v.GetX() - max_y_v.GetX())) * (1.f / dY);
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

geometry::Vector4d rendering::Gradients::calc_step(Vector4d* vec, const Vertex& min_y_v, const Vertex& mid_y_v, const Vertex& max_y_v,
                    double diff, bool is_x) {
    auto tmp = is_x ? ((vec[1] - vec[2]) * (min_y_v.GetY() - max_y_v.GetY()) -
                        (vec[0] - vec[2]) * (mid_y_v.GetY() - max_y_v.GetY())) :
                                ((vec[1] - vec[2]) * (min_y_v.GetX() - max_y_v.GetX()) -
                                (vec[0] - vec[2]) * (mid_y_v.GetX() - max_y_v.GetX()));
    return tmp * (1.f / diff);
}