#ifndef MILKY_TINYRENDERER_VEC_H
#define MILKY_TINYRENDERER_VEC_H

#include <cmath>
#include <ostream>
#include <cassert>
#include "constriants.h"
#include "matrix.h"

template <typename V, Numeric T, size_t N> class BaseVec {
    virtual std::array<T, N> raw_data() const = 0;

public:
    virtual ~BaseVec() = default;

    V operator+(V const& other) const {
        V const& self = static_cast<V const&>(*this);
        V result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = self[i] + other[i];
        }
        return result;
    }
    
    V operator-(V const& other) const {
        V const& self = static_cast<V const&>(*this);
        V result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = self[i] - other[i];
        }
        return result;
    }
    
    template<Numeric U> V operator *(const U& n) const {
        V const& self = static_cast<V const&>(*this);
        V result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = self[i] * n;
        }
        return result;
    }
    
    template<Numeric U> V operator /(const U& n) const {
        V const& self = static_cast<V const&>(*this);
        V result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = self[i] / n;
        }
        return result;
    }
    
    V& operator+=(V const& other) {
        V& self = static_cast<V&>(*this);
        for (size_t i = 0; i < N; ++i) {
            self[i] += other[i];
        }
        return self;
    }
    
    V& operator-=(V const& other) {
        V& self = static_cast<V&>(*this);
        for (size_t i = 0; i < N; ++i) {
            self[i] -= other[i];
        }
        return self;
    }
    
    V& operator*=(T scalar) {
        V& self = static_cast<V&>(*this);
        for (size_t i = 0; i < N; ++i) {
            self[i] *= scalar;
        }
        return self;
    }
    
    V& operator/=(T scalar) {
        V& self = static_cast<V&>(*this);
        for (size_t i = 0; i < N; ++i) {
            self[i] /= scalar;
        }
        return self;
    }
    
    V operator-() const {
        V const& self = static_cast<V const&>(*this);
        V result;
        for (size_t i = 0; i < N; ++i) {
            result[i] = -self[i];
        }
        return result;
    }

    T dot(V const& other) const {
        V const& self = static_cast<V const&>(*this);
        T result = 0;
        for (size_t i = 0; i < N; ++i) {
            result += self[i] * other[i];
        }
        return result;
    }

    T norm() const {
        V const& self = static_cast<V const&>(*this);
        return std::sqrt(dot(self));
    }

    V normalized() const {
        return *this / norm();
    }

    T length() const {
        return std::sqrt(static_cast<V const&>(*this).dot(static_cast<V const&>(*this)));
    }

    VecM<T, N> column_into() const {
        V const& self = static_cast<V const&>(*this);
        std::array<std::array<T, 1>, N> result;
        for (size_t i = N; i--;) {
            result[i][0] = self[i];
        }
        return VecM<T, N>(result);
    }

    RowVecM<T, N> row_into() const {
        RowVecM<T, N> m{};
        m[0] = raw_data();

        return m;
    }
};


template <Numeric T, size_t N> class Vec final : public BaseVec<Vec<T, N>, T, N> {
    std::array<T, N> data{0};

    std::array<T, N> raw_data() const override {
        return data;
    }

public:
    Vec() = default;

    explicit Vec(std::array<T, N> brace) {
        std::copy(brace.begin(), brace.end(), data);
    }

    T& operator[](size_t i) {
        return data[i];
    }

    T const& operator[](size_t i) const {
        return data[i];
    }
};

template<Numeric T> class Vec<T, 2> final : public BaseVec<Vec<T, 2>, T, 2> {
    std::array<T, 2> raw_data() const override {
        return data;
    }

public:
    union {
        std::array<T, 2> data;
        struct {
            T x, y;
        };
    };

    Vec() : data{0} {}

    Vec(T x, T y) : x(x), y(y) {}

    explicit Vec(std::array<T, 2> brace): data(brace) {}

    T& operator[](size_t i) {
        assert(i < 2);
        return data[i];
    }

    const T& operator[](size_t i) const {
        assert(i < 2);
        return data[i];
    }
};

template<Numeric T>
class Vec<T, 3> final :
public BaseVec<Vec<T, 3>, T, 3> {
    std::array<T, 3> raw_data() const override {
        return data;
    }

public:
    union {
        std::array<T, 3> data;
        struct {
            T x, y, z;
        };
    };

    Vec() : data{0} {}

    Vec(T x, T y, T z) : x(x), y(y), z(z) {}

    explicit Vec(std::array<T, 3> brace): data(brace) {}

    T& operator[](size_t i) {
        assert(i < 3);
        return data[i];
    }

    const T& operator[](size_t i) const {
        assert(i < 3);
        return data[i];
    }

    Vec cross(const Vec& other) const {
        return {
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        };
    }

    struct View3 {
        T x, y, z;
    };
    View3 view() {
        return {x, y, z};
    }
};

template<Numeric T> class Vec<T, 4> final : public BaseVec<Vec<T, 4>, T, 4> {
    std::array<T, 4> raw_data() const override {
        return data;
    }

public:
    union {
        std::array<T, 4> data;
        struct {
            T x, y, z, w;
        };
    };

    Vec() : data{0} {}

    Vec(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

    explicit Vec(std::array<T, 4> brace): data(brace) {}

    T& operator[](size_t i) {
        assert(i < 4);
        return data[i];
    }

    const T& operator[](size_t i) const {
        assert(i < 4);
        return data[i];
    }
};

using Vec2f = Vec<float, 2>;
using Vec2i = Vec<int, 2>;
using Vec3f = Vec<float, 3>;
using Vec3d = Vec<double, 3>;
using Vec3i = Vec<int, 3>;
using Vec4f = Vec<float, 4>;
using Vec4d = Vec<double, 4>;
using Vec4i = Vec<int, 4>;

#endif