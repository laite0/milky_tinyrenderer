#ifndef MILKY_TINYRENDERER_MATRIX_CPP
#define MILKY_TINYRENDERER_MATRIX_CPP

#include "matrix.h"

template<Numeric T, size_t R, size_t C> Matrix<T, R, C>::Matrix()
: m({}) {}

template<Numeric T, size_t R, size_t C> Matrix<T, R, C>::Matrix(std::array<std::array<T, C>, R> const& initializer)
: m(initializer) {}

template<Numeric T, size_t R, size_t C> size_t Matrix<T, R, C>::n_rows() {
    return R;
}

template<Numeric T, size_t R, size_t C> size_t Matrix<T, R, C>::n_cols() {
    return C;
}

template<Numeric T, size_t R, size_t C> template<size_t N> Matrix<T, N, N> Matrix<T, R, C>::identity() {
    Matrix<T, N, N> E{};
    const T id = T(1);
    const T zero = T(0);
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            auto v = (i == j ? id : zero);
            T& ok = E[i][j];
            ok = v;
        }
    }
    return E;
}

template<Numeric T, size_t R, size_t C> std::array<T, C> & Matrix<T, R, C>::operator[](size_t i) {
    return m[i];
}

template<Numeric T, size_t R, size_t C> const std::array<T, C>& Matrix<T, R, C>::operator[](size_t i) const {
    return m[i];
}

template<Numeric T, size_t R, size_t C> template<size_t C2> Matrix<T, R, C2> Matrix<T, R, C>::operator*(const Matrix<T, C, C2> &a) const {
    Matrix<T, R, C2> result{};
    for (int i = 0; i < R; i++) {
        for (int j = 0; j < C2; j++) {
            result[i][j] = T(0);
            for (int k = 0; k < C; k++) {
                result[i][j] += m[i][k] * a[k][j];
            }
        }
    }
    return result;
}

template<Numeric T, size_t R, size_t C> Matrix<T, C, R> Matrix<T, R, C>::transpose() {
    Matrix<T, C, R> result{};
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            result[j][i] = m[i][j];
    return result;
}

template<Numeric T, size_t R, size_t C> Matrix<T, R, C> Matrix<T, R, C>::inverse() requires (R == C) {
    // augmenting the square matrix with the identity matrix of the same dimensions A => [AI]
    Matrix<T, R, C * 2> result{};
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            result[i][j] = m[i][j];
    for (int i = 0; i < R; i++)
        result[i][i + C] = 1;
    // first pass
    for (int i = 0; i < R - 1; i++) {
        // normalize the first row
        for (int j = C - 1; j >= 0; --j)
            result[i][j] /= result[i][i];
        for (int k = i + 1; k < R; k++) {
            T coeff = result[k][i];
            for (int j = 0; j < C; j++) {
                result[k][j] -= result[i][j] * coeff;
            }
        }
    }
    // normalize the last row
    for (int j = C - 1; j >= R - 1; --j)
        result[R - 1][j] /= result[R - 1][R - 1];
    // second pass
    for (size_t i = R - 1; i > 0; --i) {
        for (size_t k = i - 1; k != static_cast<size_t>(-1); --k) {
            T coeff = result[k][i];
            for (int j = 0; j < C; j++) {
                result[k][j] -= result[i][j] * coeff;
            }
        }
    }
    // cut the identity matrix back
    Matrix<T, R, C> truncate;
    for (int i = 0; i < R; i++)
        for (int j = 0; j < C; j++)
            truncate[i][j] = result[i][j + C];
    return truncate;
}

#endif