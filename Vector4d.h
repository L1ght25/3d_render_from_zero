#pragma once
#include "Matrix4d.h"

class Vector4d : public Matrix4d {
public:
    Vector4d(double x, double y, double z, double w) : Matrix4d{{x}, {y}, {z}, {w}} {

    }

    Vector4d(const Matrix<double>& mat) : Matrix4d{{mat(0, 0)}, {mat(1, 0)}, {mat(2, 0)}, {mat(3, 0)}} {
        if (mat.Columns() != 1) {
            throw std::invalid_argument("Invalid input: is not vector");
        }
    }

    double GetX() const {
        return matrix_[0][0];
    }

    double GetY() const {
        return matrix_[1][0];
    }

    double GetZ() const {
        return matrix_[2][0];
    }

    double GetW() const {
        return matrix_[3][0];
    }

    Vector4d Transform(const Matrix4d& oper) const {
        return Mut(oper, *this);
//        return Vector4d{oper * (*dynamic_cast<const Matrix<double>*>(this))};
    }

    friend class Vertex;
};