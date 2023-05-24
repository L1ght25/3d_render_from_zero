#pragma once
#include "4d_primitives.h"

namespace geometry {
class Vertex {
  public:
    Vertex(double x, double y, double z);
    Vertex(double x, double y, double z, Vector4d texture, Vector4d normal);
    Vertex(Vector4d vec, Vector4d texture, Vector4d normal);

    void SetTexturePos(Vector4d pos);
    void SetNormalPos(Vector4d pos);

    double GetX() const;
    double GetY() const;
    double GetZ() const;
    double GetW() const;

    const Vector4d& GetPos() const;

    Vector4d GetTexturePos() const;
    Vector4d GetNormal() const;

    Vertex TransformBy(const Matrix4d& oper) const;
    Vertex TransformBy(const Matrix4d& full_transform, const Matrix4d& obj_transform) const;

    Vertex PerspectiveDivision();

    Vertex GetLinearInterpolationOfVertices(const Vertex& another, const double& coef) const;

    double SquareTriangleTwice(const Vertex& second_v, const Vertex& third_v) const;

    bool IsInsideView() const;

  private:
    Vector4d pos_;
    Vector4d texture_pos_;
    Vector4d normal_;
};
}  // namespace geometry
