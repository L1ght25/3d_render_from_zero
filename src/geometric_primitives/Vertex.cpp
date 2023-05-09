#include "Vertex.h"

geometry::Vertex::Vertex(double x, double y, double z) : pos_(x, y, z, 1), texture_pos_(0, 0, 0, 0) {
}

geometry::Vertex::Vertex(double x, double y, double z, geometry::Vector4d texture) : pos_(x, y, z, 1), texture_pos_(std::move(texture)) {
}

geometry::Vertex::Vertex(geometry::Vector4d vec, geometry::Vector4d texture) : pos_(std::move(vec)), texture_pos_(std::move(texture)) {
}

void geometry::Vertex::SetTexturePos(geometry::Vector4d pos) {
    texture_pos_ = std::move(pos);
}

double geometry::Vertex::GetX() const {
    return pos_.GetX();
}

double geometry::Vertex::GetY() const {
    return pos_.GetY();
}

double geometry::Vertex::GetZ() const {
    return pos_.GetZ();
}

double geometry::Vertex::GetW() const {
    return pos_.GetW();
}

const geometry::Vector4d& geometry::Vertex::GetPos() const {
    return pos_;
}

geometry::Vector4d geometry::Vertex::GetTexturePos() const {
    return texture_pos_;
}

geometry::Vertex geometry::Vertex::Transform(const geometry::Matrix4d& oper) const {
    return {pos_.Transform(oper), texture_pos_};
}

geometry::Vertex geometry::Vertex::PerspectiveDivision() {
    return {geometry::Vector4d{pos_.GetX() / pos_.GetW(), pos_.GetY() / pos_.GetW(), pos_.GetZ() / pos_.GetW(), pos_.GetW()}, texture_pos_};
}

geometry::Vertex geometry::Vertex::LinearInterpolation(const Vertex& another, const double& coef) const {
    return {pos_.LinearInterpolation(another.pos_, coef), texture_pos_.LinearInterpolation(another.texture_pos_, coef)};
}

double geometry::Vertex::SquareTriangleTwice(const geometry::Vertex& second_v, const geometry::Vertex& third_v) const {
    return (second_v.GetX() - GetX()) * (third_v.GetY() - GetY()) - (third_v.GetX() - GetX()) * (second_v.GetY() - GetY());
}

bool geometry::Vertex::IsInsideView() const {
    bool is_inside = true;
    for (int i = 0; i < 3; ++i) {
        is_inside &= -GetW() <= GetPos()[i] && GetPos()[i] <= GetW(); 
    }
    return is_inside;
}