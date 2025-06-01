#ifndef MILKY_TINYRENDERER_MATRIX_H
#define MILKY_TINYRENDERER_MATRIX_H

#include <array>
#include <ostream>
#include "constriants.h"

constexpr int DEFAULT_DIMENSIONS=4;

template<Numeric T, size_t R = DEFAULT_DIMENSIONS, size_t C = DEFAULT_DIMENSIONS> class Matrix {
    std::array<std::array<T, C>, R> m;
public:
    Matrix();

    explicit Matrix(std::array<std::array<T, C>, R> const& initializer);

    [[nodiscard]] static size_t n_rows();

    [[nodiscard]] static size_t n_cols();

    template<size_t N> static Matrix<T, N, N> identity();

    std::array<T, C>& operator[](size_t i);

    const std::array<T, C>& operator[](size_t i) const;

    template<size_t C2> Matrix<T, R, C2> operator*(const Matrix<T, C, C2>& a) const;

    Matrix<T, C, R> transpose();

    Matrix inverse() requires(R == C);

    friend std::ostream& operator<<<>(std::ostream& s, Matrix& m);
};

using Mat3x3f = Matrix<float, 3, 3>;
using Mat4x4f = Matrix<float>;
template<Numeric T, size_t N> using VecM = Matrix<T, N, 1>;
template<size_t N> using VecMf = VecM<float, N>;
template<Numeric T, size_t N> using RowVecM = Matrix<T, 1, N>;
template<size_t N> using RowVecMf = RowVecM<float, N>;

template<Numeric T, size_t R, size_t C> inline std::ostream& operator<<(std::ostream& s, Matrix<T, R, C>& m) {
    for (int i=0; i<m.n_rows(); i++)  {
        for (int j=0; j<m.n_cols(); j++) {
            s << m[i][j];
            if (j<m.n_cols()-1) s << "\t";
        }
        s << "\n";
    }
    return s;
}

template <Numeric T, size_t R, size_t C> Matrix<T, R, C> operator*(T scalar, const Matrix<T, R, C>& mat) {
    return mat * scalar;
}

#include "matrix.cpp"

#endif //MILKY_TINYRENDERER_MATRIX_H
