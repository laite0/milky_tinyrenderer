#ifndef MILKY_TINYRENDERER_MATRIX_H
#define MILKY_TINYRENDERER_MATRIX_H

#include <vector>
#include <ostream>
#include "vec.h"

const int DEFAULT_DIMENSIONS=4;

class Matrix {
    std::vector<std::vector<float>> m;
    int rows, cols;
public:
    Matrix(int r=DEFAULT_DIMENSIONS, int c=DEFAULT_DIMENSIONS);

    inline int n_rows() const;

    inline int n_cols() const;

    static Matrix identity(int dimensions);

    std::vector<float>& operator[](const int i);

    Matrix operator*(const Matrix& a) const;

    Matrix transpose();

    Matrix inverse();

    friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};

inline std::ostream& operator<<(std::ostream& s, Matrix& m) {
    for (int i=0; i<m.n_rows(); i++)  {
        for (int j=0; j<m.n_cols(); j++) {
            s << m[i][j];
            if (j<m.n_cols()-1) s << "\t";
        }
        s << "\n";
    }
    return s;
}

#endif //MILKY_TINYRENDERER_MATRIX_H
