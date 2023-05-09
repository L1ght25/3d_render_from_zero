#pragma once
#include "MatrixBase.h"

namespace geometry {
class Matrix4d : public Matrix<double> {  // special matrix that is used in most operations in 3d-render
  public:
    Matrix4d() : Matrix<double>(4, 4) {
    }

    Matrix4d(std::initializer_list<std::initializer_list<double>> matrix) : Matrix<double>(matrix) {
    }

    Matrix4d(std::vector<std::vector<double>>&& vec) noexcept : Matrix<double>(std::move(vec)) {
    }

    Matrix4d(const Matrix<double>& another) : Matrix<double>(another) {
    }

    Matrix4d& InitIdentityOperator();

    Matrix4d& InitTranslationOperator(double x, double y, double z);

    Matrix4d& InitScreenSpaceTransform(double half_height, double half_width);

    Matrix4d& InitRotation(double x, double y, double z, double alpha);

    Matrix4d& InitRotation(double x, double y, double z);

    Matrix4d& InitPerspective(double fov, double aspect, double z_near, double z_far);
};

class Vector4d final : public Matrix4d {
  public:
    Vector4d() = default;

    Vector4d(double x, double y, double z, double w);

    Vector4d(const Matrix<double>& mat);

    double GetX() const;

    double GetY() const;

    double GetZ() const;

    double GetW() const;

    double operator[](size_t i) const;

    Vector4d Transform(const Matrix4d& oper) const;

    Vector4d operator*(double alpha) const;

    Vector4d LinearInterpolation(const Vector4d &another, const double& coef) const;

    friend class Vertex;
};
}  // namespace geometry