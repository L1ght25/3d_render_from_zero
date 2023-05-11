#include "Edge.h"

rendering::Edge::Edge(const Gradients& grad, const Vertex& first, const Vertex& second, int ind_of_min_v_color)
    : y_start_(std::ceil(first.GetY())), y_end_(std::ceil(second.GetY())), x_step_((first.GetX() - second.GetX()) / (first.GetY() - second.GetY())),
      cur_x_(std::ceil(first.GetX()) + x_step_ * (std::ceil(first.GetY()) - first.GetY())),

      cur_texture_pos_(grad.GetTexPos(ind_of_min_v_color) + grad.GetYStep() * (std::ceil(first.GetY()) - first.GetY()) +
                       grad.GetXStep() * (cur_x_ - first.GetX())),
      texture_pos_step_(grad.GetYStep() + grad.GetXStep() * x_step_),

      cur_depth_(grad.GetDepthPos(ind_of_min_v_color) + grad.GetDepthStepY() * (std::ceil(first.GetY()) - first.GetY()) +
                 grad.GetDepthStepX() * (cur_x_ - first.GetX())),
      depth_step_(grad.GetDepthStepY() + grad.GetDepthStepX() * x_step_),

      cur_one_over_z_(grad.GetOneOverZPos(ind_of_min_v_color) + grad.GetOneOverZY() * (std::ceil(first.GetY()) - first.GetY()) +
                      grad.GetOneOverZX() * (cur_x_ - first.GetX())),
      cur_z_step_(grad.GetOneOverZY() + grad.GetOneOverZX() * x_step_),
      cur_light_(grad.GetLightPos(ind_of_min_v_color) + grad.GetLightYStep() * (std::ceil(first.GetY()) - first.GetY()) +
                 grad.GetLightXStep() * (cur_x_ - first.GetX())),
      cur_light_step_(grad.GetLightYStep() + grad.GetLightXStep() * x_step_) {
}

void rendering::Edge::Step() {
    cur_x_ += x_step_;
    cur_texture_pos_ += texture_pos_step_;
    cur_depth_ += depth_step_;
    cur_one_over_z_ += cur_z_step_;
    cur_light_ += cur_light_step_;
}

const geometry::Vector4d& rendering::Edge::GetTexturePos() const {
    return cur_texture_pos_;
}

double rendering::Edge::GetX() const {
    return cur_x_;
}

int rendering::Edge::GetYStart() const {
    return y_start_;
}

int rendering::Edge::GetYEnd() const {
    return y_end_;
}

double rendering::Edge::GetDepth() const {
    return cur_depth_;
}

double rendering::Edge::GetOneOverZ() const {
    return cur_one_over_z_;
}

double rendering::Edge::GetLight() const {
    return cur_light_;
}