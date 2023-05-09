#pragma once
#include "4d_primitives.h"

namespace geometry {
class Vertex {
  public:
    Vertex(double x, double y, double z);

    Vertex(double x, double y, double z, Vector4d color);

    Vertex(Vector4d vec, Vector4d color);

    void SetTexturePos(Vector4d pos);

    double GetX() const;

    double GetY() const;

    double GetZ() const;

    double GetW() const;

    const Vector4d& GetPos() const;

    Vector4d GetTexturePos() const;

    Vertex Transform(const Matrix4d& oper) const;

    Vertex PerspectiveDivision();

    double SquareTriangleTwice(const Vertex& second_v, const Vertex& third_v) const;

  private:
    Vector4d pos_;
    Vector4d texture_pos_;
};
}  // namespace geometry