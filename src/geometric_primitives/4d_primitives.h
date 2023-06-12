#pragma once
#include "MatrixBase.h"

namespace geometry {
class Matrix4d : public Matrix<double> {  // special matrix that is used in most operations in 3d-render
  public:
    Matrix4d() : Matrix<double>(4, 4) {
    }

    Matrix4d(std::initializer_list<std::initializer_list<double>> matrix) : Matrix<double>(matrix) {
    }

    // Matrix4d(std::vector<std::vector<double>>&& vec) noexcept : Matrix<double>(std::move(vec)) {
    // }

    Matrix4d(const Matrix<double>& another) : Matrix<double>(another) {
    }

    static Matrix4d InitIdentityOperator();

    static Matrix4d InitTranslationOperator(double x, double y, double z);

    static Matrix4d InitScreenSpaceTransform(double half_height, double half_width);

    static Matrix4d InitRotation(double x, double y, double z, double alpha);

    static Matrix4d InitRotation(double x, double y, double z);

    static Matrix4d InitPerspective(double fov, double aspect, double z_near, double z_far);
};

class Vector4d final : public Matrix<double> {
  public:
    Vector4d();

    Vector4d(double x, double y, double z, double w);

    Vector4d(const Matrix<double>& mat);

    double GetX() const;

    double GetY() const;

    double GetZ() const;

    double GetW() const;

    double operator[](size_t i) const;

    double& operator[](size_t i);

    static Vector4d TransformVectorByOperator(const Vector4d& vec, const Matrix4d& oper);

    Vector4d operator*(double alpha) const;

    Vector4d Normalize();

    Vector4d CrossProduct(const Vector4d& another) const;

    double Length() const;

    Vector4d LinearInterpolationBetweenDots(const Vector4d& another, double coef) const;
};
}  // namespace geometry
