#pragma once
#include <cmath>
#include <cstdio>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <system_error>
#include <vector>

namespace geometry {
// simple implementation of matrix
template <class T> class Matrix {
  public:
    Matrix(size_t h, size_t w) : matrix_(h, w) {
    }

    Matrix(std::initializer_list<std::initializer_list<T>> matrix) : matrix_(matrix) {

    }

    T& operator[](size_t i, size_t j) {
        return matrix_[i, j];
    }

    T operator[](size_t i, size_t j) const {
        return matrix_[i, j];
    }
 
    Matrix operator*(double alpha) const {
        Matrix new_matrix(Rows(), Columns());
        for (size_t i = 0; i < Rows(); ++i) {
            for (size_t j = 0; j < Columns(); ++j) {
                new_matrix[i, j] = matrix_[i, j] * alpha;
            }
        }
        return new_matrix;
    }

    Matrix operator*=(double alpha) {
        for (size_t i = 0; i < Rows(); ++i) {
            for (size_t j = 0; j < Columns(); ++j) {
                matrix_[i, j] *= alpha;
            }
        }
        return *this;
    }

    Matrix operator*(const Matrix<T>& another) const {
        Matrix<T> ans(Rows(), another.Columns());
        for (size_t i = 0; i < Rows(); ++i) {
            for (size_t k = 0; k < another.Columns(); ++k) {
                T sum = 0;
                for (size_t j = 0; j < Columns(); ++j) {
                    sum += matrix_[i, j] * another.matrix_[j, k];
                }
                ans[i, k] = sum;
            }
        }
        return ans;
    }

    Matrix operator*=(const Matrix<T>& another) {
        SingleVectorMatrixImpl new_matrix_(Rows(), std::vector<T>(another.Columns()));
        for (size_t i = 0; i < Rows(); ++i) {
            for (size_t k = 0; k < another.Columns(); ++k) {
                T sum = 0;
                for (size_t j = 0; j < another.Columns(); ++j) {
                    sum += matrix_[i, j] * another.matrix_[j, k];
                }
                new_matrix_[i, k] = sum;
            }
        }
        matrix_ = std::move(new_matrix_);
        return *this;
    }

    Matrix& operator+=(const Matrix<T>& another) {
        CheckSizesAndThrow(*this, another);
        for (size_t i = 0; i < Rows(); ++i) {
            for (size_t j = 0; j < Columns(); ++j) {
                matrix_[i, j] += another[i, j];
            }
        }
        return *this;
    }

    Matrix& operator-=(const Matrix<T>& another) {
        CheckSizesAndThrow(*this, another);
        for (size_t i = 0; i < Rows(); ++i) {
            for (size_t j = 0; j < Columns(); ++j) {
                matrix_[i, j] -= another[i, j];
            }
        }
        return *this;
    }

    Matrix operator+(const Matrix<T>& matrix_2) const {
        auto result = *this;
        return result += matrix_2;
    }

    Matrix operator-(const Matrix<T>& matrix_2) const {
        auto result = *this;
        return result -= matrix_2;
    }

    static void CheckSizesAndThrow(const Matrix<T>& matrix_1, const Matrix<T>& matrix_2) {
        if (matrix_1.Rows() != matrix_2.Rows() or matrix_1.Columns() != matrix_2.Columns()) {
            fprintf(stderr, "Got size (%zu, %zu), but expected: (%zu, %zu)", matrix_2.Rows(), matrix_2.Columns(), matrix_1.Rows(),
                    matrix_1.Columns());
            throw std::runtime_error("invalid operation length");
        }
    }

    size_t Rows() const {
        return matrix_.h_;
    }

    size_t Columns() const {
        return matrix_.w_;
    }

  protected:
    // single-vector matrix details
    struct SingleVectorMatrixImpl {
        SingleVectorMatrixImpl(size_t h, size_t w) : h_(h), w_(w), matrix_(h_ * w_) {

        }

        SingleVectorMatrixImpl(std::initializer_list<std::initializer_list<T>> data) : h_(data.size()), w_(data.begin()->size()) {
            matrix_.resize(h_ * w_);
            size_t i = 0, j = 0;
            for (auto&& row : data) {
                j = 0;
                for (auto&& element : row) {
                    operator[](i, j) = element;
                    ++j;
                }
                ++i;
            }
        }

        T& operator[](size_t i, size_t j) {
            return matrix_[i * w_ + j];
        }

        T operator[](size_t i, size_t j) const {
            return matrix_[i * w_ + j];
        }
        size_t h_;
        size_t w_;
        std::vector<T> matrix_;
    };

    SingleVectorMatrixImpl matrix_;
};
}  // namespace geometry
