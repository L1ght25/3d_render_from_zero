#include "4d_primitives.h"

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
    matrix_[0][3] = half_width;
    matrix_[1][3] = half_height;
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

geometry::Vector4d::Vector4d(const Matrix<double>& mat) : Matrix4d{{mat(0, 0)}, {mat(1, 0)}, {mat(2, 0)}, {mat(3, 0)}} {
    if (mat.Columns() != 1) {
        fprintf(stderr, "WARNING! Got size: (%zu, %zu)\n", mat.Rows(), mat.Columns());
        throw std::invalid_argument("Invalid input: is not vector");
    }
}
