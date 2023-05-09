#pragma once
#include <cmath>
#include <iostream>
#include <limits>
#include <system_error>
#include <vector>

namespace geometry {
// simple implementation of matrix
template <class T> class Matrix {
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

    Matrix operator*(const Matrix<T>& another) const {
        Matrix<T> ans(h_, another.w_);
        for (size_t i = 0; i < h_; ++i) {
            for (size_t k = 0; k < another.w_; ++k) {
                T sum = 0;
                for (size_t j = 0; j < w_; ++j) {
                    sum += matrix_[i][j] * another.matrix_[j][k];
                }
                ans(i, k) = sum;
            }
        }
        return std::move(ans);
    }

    Matrix operator*=(const Matrix<T>& another) {
        std::vector<std::vector<T>> new_matrix_(h_, std::vector<T>(another.w_));
        for (size_t i = 0; i < h_; ++i) {
            for (size_t k = 0; k < another.w_; ++k) {
                T sum = 0;
                for (size_t j = 0; j < w_; ++j) {
                    sum += matrix_[i][j] * another.matrix_[j][k];
                }
                new_matrix_[i][k] = sum;
            }
        }
        w_ = another.w_;
        matrix_ = std::move(new_matrix_);
        return *this;
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

  protected:
    size_t h_;
    size_t w_;
    std::vector<std::vector<T>> matrix_;
};
}  // namespace geometry