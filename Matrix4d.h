#pragma once
#include "Vector4d.h"
#include <system_error>
#include <limits>
#include <vector>
#include <iostream>

template <class T>
struct Base {
    const T& Impl() const {
        return static_cast<const T&>(*this);
    }
};

template <class L, class R>
struct Glue : public Base<Glue<L, R>> {
    Glue(const L& l, const R& r) : left(l), right(r) {
    }

    const L& left;
    const R& right;
};

template <class L, class R>
constexpr Glue<L, R> operator*(const Base<L>& left, const Base<R>& right) {
    return Glue<L, R>(left.Impl(), right.Impl());
}

template <class T>
class Matrix : public Base<Matrix<T>> {
public:
    Matrix(size_t h, size_t w) : matrix_(h, std::vector<T>(w)), h_(h), w_(w) {
    }

    Matrix(std::vector<std::vector<double>> vec) : matrix_(std::move(vec)), h_(vec.size()), w_(vec[0].size()) {

    }

    Matrix(std::initializer_list<std::initializer_list<T>> matrix) : h_(matrix.size()) {
        matrix_.reserve(h_);
        for (auto&& vec : matrix) {
            matrix_.template emplace_back(vec);
        }
        w_ = matrix_[0].size();
    }

    template <class L, class R>
    Matrix(const Glue<L, R>& tree) {
        const size_t count = Count<L, R, T>(tree);
        const Matrix<T>* operands[count];
        AddMatrix<L, R, T>(tree, operands, 0);
        size_t sizes[count + 1];
        EvalSizes<T>(operands, sizes, count);
        //        for (size_t i = 0; i < count; ++i) {
        //            std::cout << sizes[i] << ' ' << sizes[i + 1] << std::endl;
        //        }
        std::vector<std::vector<size_t>> orders(count + 1, std::vector<size_t>(count + 1));
        std::vector<std::vector<uint64_t>> m(count + 1,
                                             std::vector<uint64_t>(count + 1, UINT64_MAX));
        for (size_t i = 0; i < count + 1; ++i) {
            m[i][i] = 0;
        }
        EvalOrders(0, count - 1, m, sizes, orders);
        Matrix<T> ans = ComputeMul(0, count - 1, operands, orders);
        matrix_ = std::move(ans.matrix_);
        h_ = ans.h_;
        w_ = ans.w_;
    }

    T& operator()(size_t i, size_t j) {
        return matrix_[i][j];
    }

    T operator()(size_t i, size_t j) const {
        return matrix_[i][j];
    }

    friend Matrix<T> operator+(const Matrix<T>& first, const Matrix<T>& matrix_2) {
        Check(first, matrix_2);
        Matrix new_matrix(first.h_, first.w_);
        for (size_t i = 0; i < first.h_; ++i) {
            for (size_t j = 0; j < first.w_; ++j) {
                new_matrix(i, j) = first.matrix_[i][j] + matrix_2(i, j);
            }
        }
        return std::move(new_matrix);
    }

    Matrix operator-(const Matrix<T>& matrix_2) const {
        Check(*this, matrix_2);
        Matrix new_matrix(h_, w_);
        for (size_t i = 0; i < h_; ++i) {
            for (size_t j = 0; j < w_; ++j) {
                new_matrix(i, j) = matrix_[i][j] - matrix_2(i, j);
            }
        }
        return std::move(new_matrix);
    }

    Matrix operator*(double alpha) const {
        Matrix new_matrix(h_, w_);
        for (size_t i = 0; i < h_; ++i) {
            for (size_t j = 0; j < w_; ++j) {
                new_matrix(i, j) = matrix_[i][j] * alpha;
            }
        }
        return new_matrix;
    }

    Matrix& operator+=(const Matrix<T>& another) {
        Check(*this, another);
        for (size_t i = 0; i < h_; ++i) {
            for (size_t j = 0; j < w_; ++j) {
                matrix_[i][j] += another(i, j);
            }
        }
        return *this;
    }

    static void Check(const Matrix<T>& matrix_1, const Matrix<T>& matrix_2) {
        if (matrix_1.h_ != matrix_2.h_ or matrix_1.w_ != matrix_2.w_) {
            throw std::runtime_error("invalid operation length");
        }
    }

    size_t Rows() const {
        return h_;
    }

    size_t Columns() const {
        return w_;
    }

//    Vector4d Transform(const Vector4d& vec) const {
//        return *this * vec;
//    }

protected:
    Matrix<T> Mut(const Matrix<T>& first, const Matrix<T>& second) const {
        Matrix<T> ans(first.h_, second.w_);
        for (size_t i = 0; i < first.h_; ++i) {
            for (size_t k = 0; k < second.w_; ++k) {
                T sum = 0;
                for (size_t j = 0; j < first.w_; ++j) {
                    sum += first.matrix_[i][j] * second.matrix_[j][k];
                }
                ans(i, k) = sum;
            }
        }
        return std::move(ans);
    }

    uint64_t EvalOrders(int i, int j, std::vector<std::vector<uint64_t>>& m, size_t sizes[],
                        std::vector<std::vector<size_t>>& orders) {
        if (m[i][j] == UINT64_MAX) {
            for (int k = i; k < j; k++) {
                uint64_t temp = EvalOrders(i, k, m, sizes, orders) +
                                EvalOrders(k + 1, j, m, sizes, orders) +
                                sizes[i] * sizes[k + 1] * sizes[j + 1];
                if (temp < m[i][j]) {
                    m[i][j] = temp;
                    orders[i][j] = k;
                }
            }
        }
        return m[i][j];
    }

    Matrix<T> ComputeMul(size_t left, size_t right, const Matrix<T>* operands[],
                         std::vector<std::vector<size_t>>& orders) {
        if (left == right) {
            return *operands[left];
        }
        return Mut(ComputeMul(left, orders[left][right], operands, orders),
                   ComputeMul(orders[left][right] + 1, right, operands, orders));
    }

    size_t h_;
    size_t w_;
    std::vector<std::vector<T>> matrix_;
};

template <class L, class R, class T>
constexpr size_t Count(Glue<L, R> root) {
    size_t ans = 0;
    if constexpr (std::is_same_v<std::decay_t<decltype(root.left)>, Matrix<T>>) {
        ans += 1;
    } else {
        ans += Count<std::decay_t<decltype(root.left.left)>,
                std::decay_t<decltype(root.left.right)>, T>(root.left);
    }
    if constexpr (std::is_same_v<std::decay_t<decltype(root.right)>, Matrix<T>>) {
        ans += 1;
    } else {
        ans += Count<std::decay_t<decltype(root.right.left)>,
                std::decay_t<decltype(root.right.right)>, T>(root.right);
    }
    return ans;
}

template <class L, class R, class T>
constexpr size_t AddMatrix(Glue<L, R> root, const Matrix<T>* operands[], size_t curr_ind) {
    if constexpr (std::is_same_v<std::decay_t<decltype(root.left)>, Matrix<T>>) {
        operands[curr_ind] = &root.left;
        ++curr_ind;
    } else {
        curr_ind =
                AddMatrix<std::decay_t<decltype(root.left.left)>,
                        std::decay_t<decltype(root.left.right)>, T>(root.left, operands, curr_ind);
    }
    if constexpr (std::is_same_v<std::decay_t<decltype(root.right)>, Matrix<T>>) {
        operands[curr_ind] = &root.right;
        ++curr_ind;
    } else {
        curr_ind =
                AddMatrix<std::decay_t<decltype(root.right.left)>,
                        std::decay_t<decltype(root.right.right)>, T>(root.right, operands, curr_ind);
    }
    return curr_ind;
}

template <class T>
constexpr void EvalSizes(const Matrix<T>* operands[], size_t sizes[], size_t len) {
    for (size_t i = 0; i < len; ++i) {
        const Matrix<T>& mat = *(operands[i]);
        if (i != 0 and mat.Rows() != sizes[i]) {
            throw std::runtime_error("invalid size");
        }
        sizes[i] = mat.Rows();
        sizes[i + 1] = mat.Columns();
    }
}


class Matrix4d : public Matrix<double> {
        public:
            Matrix4d() : Matrix<double>(4, 4) {
            }

            Matrix4d(std::initializer_list<std::initializer_list<double>> matrix) : Matrix<double>(matrix) {

            }

            Matrix4d(const std::vector<std::vector<double>>& vec) : Matrix<double>(vec) {

            }

            Matrix4d(const Matrix<double>& another) : Matrix<double>(another) {

            }

            Matrix4d& InitIdentityOperator() {
                for (size_t i = 0; i < 4; ++i) {
                    matrix_[i][i] = 1;
                }
                return *this;
            }

            Matrix4d& InitTranslationOperator(double x, double y, double z) {
                InitIdentityOperator();
                matrix_[0][3] = x;
                matrix_[1][3] = y;
                matrix_[2][3] = z;
                return *this;
            }

            Matrix4d& InitScreenSpaceTransform(double half_height, double half_width) {
                InitIdentityOperator();
                matrix_[0][0] = half_width;
                matrix_[1][1] = -half_height;
                matrix_[0][3] = half_width;
                matrix_[1][3] = half_height;
                return *this;
            }

            Matrix4d& InitRotation(double x, double y, double z, double alpha) {  // поворот вокруг произвольной оси
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

            Matrix4d& InitRotation(double x, double y, double z) {  // композиция поворотов вокруг осей
                std::vector<std::vector<double>>rotate_x(4, std::vector<double>(4));
                std::vector<std::vector<double>>rotate_y(4, std::vector<double>(4));
                std::vector<std::vector<double>>rotate_z(4, std::vector<double>(4));

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

                *this = std::move(Matrix4d(rotate_z) * Matrix4d(rotate_y) * Matrix4d(rotate_x));
                matrix_[3][3] = 1;
                return *this;
            }

            Matrix4d& InitPerspective(double fov, double aspect, double z_near, double z_far) {
                double tan_half = std::tan(fov / 2);
                double z_range = z_near - z_far;

                matrix_[0][0] = (double)1 / (tan_half * aspect);
                matrix_[1][1] = (double)1 / tan_half;
                matrix_[2][2] = - (z_near + z_far) / z_range;
                matrix_[2][3] = 2 * z_far * z_near / z_range;
                matrix_[3][2] = 1;

                return *this;
            }

            Matrix4d operator*(const Matrix4d& another) {
                return Mut(*this, another);
            }

        };

