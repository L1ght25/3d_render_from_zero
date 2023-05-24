#include "Vertex.h"
#include "4d_primitives.h"

geometry::Vertex::Vertex(double x, double y, double z) : pos_(x, y, z, 1), texture_pos_(0, 0, 0, 0) {
}

geometry::Vertex::Vertex(double x, double y, double z, geometry::Vector4d texture, geometry::Vector4d normal)
    : pos_(x, y, z, 1), texture_pos_(std::move(texture)), normal_(std::move(normal)) {
}

geometry::Vertex::Vertex(geometry::Vector4d vec, geometry::Vector4d texture, geometry::Vector4d normal)
    : pos_(std::move(vec)), texture_pos_(std::move(texture)), normal_(std::move(normal)) {
}

void geometry::Vertex::SetTexturePos(geometry::Vector4d pos) {
    texture_pos_ = std::move(pos);
}

void geometry::Vertex::SetNormalPos(geometry::Vector4d pos) {
    normal_ = std::move(pos);
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

geometry::Vector4d geometry::Vertex::GetNormal() const {
    return normal_;
}

geometry::Vertex geometry::Vertex::TransformBy(const geometry::Matrix4d& oper) const {
    return {pos_.Transform(oper), texture_pos_, normal_};
}

geometry::Vertex geometry::Vertex::TransformBy(const geometry::Matrix4d& full_transform, const geometry::Matrix4d& obj_transform) const {
    return {pos_.Transform(full_transform), texture_pos_, normal_.Transform(obj_transform)};
}

geometry::Vertex geometry::Vertex::PerspectiveDivision() {
    return {geometry::Vector4d{pos_.GetX() / pos_.GetW(), pos_.GetY() / pos_.GetW(), pos_.GetZ() / pos_.GetW(), pos_.GetW()}, texture_pos_, normal_};
}

geometry::Vertex geometry::Vertex::GetLinearInterpolationOfVertices(const Vertex& another, const double& coef) const {
    return {pos_.LinearInterpolationBetweenDots(another.pos_, coef), texture_pos_.LinearInterpolationBetweenDots(another.texture_pos_, coef),
            normal_.LinearInterpolationBetweenDots(another.normal_, coef)};
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
