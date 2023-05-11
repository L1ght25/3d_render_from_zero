#include "4d_primitives.h"
#include <cmath>

geometry::Matrix4d& geometry::Matrix4d::InitIdentityOperator() {
    for (size_t i = 0; i < 4; ++i) {
        matrix_[i][i] = 1;
    }
    return *this;
}

geometry::Matrix4d& geometry::Matrix4d::InitTranslationOperator(double x, double y, double z) {
    InitIdentityOperator();
    matrix_[0][3] = x;
    matrix_[1][3] = y;
    matrix_[2][3] = z;
    return *this;
}

geometry::Matrix4d& geometry::Matrix4d::InitScreenSpaceTransform(double half_height, double half_width) {
    InitIdentityOperator();
    matrix_[0][0] = half_width;
    matrix_[1][1] = -half_height;
    matrix_[0][3] = half_width - 0.5;  // for accuracy
    matrix_[1][3] = half_height - 0.5;
    return *this;
}

geometry::Matrix4d& geometry::Matrix4d::InitRotation(double x, double y, double z, double alpha) {  // поворот вокруг произвольной оси
    double sin_alpha = std::sin(alpha);
    double cos_alpha = std::cos(alpha);
    double cos_inv = 1 - cos_alpha;
    matrix_[0][0] = cos_alpha + x * x * cos_inv;
    matrix_[0][1] = x * y * cos_inv - z * sin_alpha;
    matrix_[0][2] = x * z * cos_inv + y * sin_alpha;
    matrix_[1][0] = x * y * cos_inv + z * sin_alpha;
    matrix_[1][1] = cos_alpha + y * y * cos_inv;
    matrix_[1][2] = y * z * cos_inv - x * sin_alpha;
    matrix_[2][0] = x * z * cos_inv - y * sin_alpha;
    matrix_[2][1] = y * z * cos_inv + x * sin_alpha;
    matrix_[2][2] = cos_alpha + z * z * cos_inv;
    matrix_[3][3] = 1;
    return *this;
}

geometry::Matrix4d& geometry::Matrix4d::InitRotation(double x, double y, double z) {  // композиция поворотов вокруг осей
    std::vector<std::vector<double>> rotate_x(4, std::vector<double>(4));
    std::vector<std::vector<double>> rotate_y(4, std::vector<double>(4));
    std::vector<std::vector<double>> rotate_z(4, std::vector<double>(4));

    rotate_x[0][0] = 1;
    rotate_x[1][1] = cos(x);
    rotate_x[1][2] = -sin(x);
    rotate_x[2][1] = sin(x);
    rotate_x[2][2] = cos(x);

    rotate_y[1][1] = 1;
    rotate_y[0][0] = cos(y);
    rotate_y[0][2] = sin(y);
    rotate_y[2][0] = -sin(y);
    rotate_y[2][2] = cos(y);

    rotate_z[2][2] = 1;
    rotate_z[0][0] = cos(z);
    rotate_z[0][1] = -sin(z);
    rotate_z[1][0] = sin(z);
    rotate_z[1][1] = cos(z);

    *this = std::move(Matrix4d(std::move(rotate_z)) * Matrix4d(std::move(rotate_y)) * Matrix4d(std::move(rotate_x)));
    matrix_[3][3] = 1;
    return *this;
}

geometry::Matrix4d& geometry::Matrix4d::InitPerspective(double fov, double aspect, double z_near, double z_far) {
    double tan_half = std::tan(fov / 2);
    double z_range = z_near - z_far;

    matrix_[0][0] = (double)1 / (tan_half * aspect);
    matrix_[1][1] = (double)1 / tan_half;
    matrix_[2][2] = -(z_near + z_far) / z_range;
    matrix_[2][3] = 2 * z_far * z_near / z_range;
    matrix_[3][2] = 1;

    return *this;
}

geometry::Vector4d::Vector4d(double x, double y, double z, double w) : Matrix4d{{x}, {y}, {z}, {w}} {
}

geometry::Vector4d::Vector4d(const Matrix<double>& mat) : Matrix4d{{mat(0, 0)}, {mat(1, 0)}, {mat(2, 0)}, {mat(3, 0)}} {
    if (mat.Columns() != 1) {
        fprintf(stderr, "WARNING! Got size: (%zu, %zu)\n", mat.Rows(), mat.Columns());
        throw std::invalid_argument("Invalid input: is not vector");
    }
}

double geometry::Vector4d::GetX() const {
    return matrix_[0][0];
}

double geometry::Vector4d::GetY() const {
    return matrix_[1][0];
}

double geometry::Vector4d::GetZ() const {
    return matrix_[2][0];
}

double geometry::Vector4d::GetW() const {
    return matrix_[3][0];
}

double geometry::Vector4d::operator[](size_t i) const {
    return matrix_[i][0];
}

double& geometry::Vector4d::operator[](size_t i) {
    return matrix_[i][0];
}

geometry::Vector4d geometry::Vector4d::Transform(const Matrix4d& oper) const {
    return oper * (*this);
}

geometry::Vector4d geometry::Vector4d::operator*(double alpha) const {
    return {GetX() * alpha, GetY() * alpha, GetZ() * alpha, GetW() * alpha};
}

geometry::Vector4d geometry::Vector4d::LinearInterpolation(const Vector4d& another, const double& coef) const {
    return *this + (another - *this) * coef;
}

double geometry::Vector4d::Length() const {
    return std::sqrt(GetX() * GetX() + GetY() * GetY() + GetZ() * GetZ() + GetW() * GetW());
}

geometry::Vector4d geometry::Vector4d::Normalize() {
    for (int i = 0; i < 3; ++i) {
        matrix_[i][0] /= Length();
    }
    return *this;
}

geometry::Vector4d geometry::Vector4d::CrossProduct(const Vector4d& another) const {
    Vector4d ans;
    ans[0] = (*this)[1] * another[2] - (*this)[2] * another[1];
    ans[1] = (*this)[2] * another[0] - (*this)[0] * another[2];
    ans[2] = (*this)[0] * another[1] - (*this)[1] * another[0];

    return ans;
}
