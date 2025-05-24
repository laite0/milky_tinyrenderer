#ifndef __VEC_H__
#define __VEC_H__

#include <cmath>
#include <ostream>

template <typename T> struct Vec2 {
    union {
        struct {
            T u;
            T v;
        };
        struct {
            T x;
            T y;
        };
        T raw[2];
    };

    Vec2() : u(0), v(0) {}
    Vec2(T _1, T _2) : u(_1), v(_2) {}

    inline Vec2<T> operator +(const Vec2<T>& other) const {
        return Vec2<T>(u + other.u, v + other.v);
    }

    inline Vec2<T> operator -(const Vec2<T>& other) const {
        return Vec2<T>(u - other.u, v - other.v);
    }

    inline Vec2<T> operator *(float f) const {
        return Vec2<T>(u * f, v * f);
    }

    template <typename> friend std::ostream& operator<<(std::ostream& stream, Vec2<T>& v);
};

template <typename T> struct Vec3 {
    union {
        struct {
            T x;
            T y;
            T z;
        };
        T raw[3];
    };

    Vec3() : x(), y(), z() {}
    Vec3(T _1, T _2, T _3) : x(_1), y(_2), z(_3) {}

    inline Vec3<T> operator +(const Vec3<T>& other) const {
        return Vec3<T>(x + other.x, y + other.y, z + other.z);
    }

    inline Vec3<T> operator -(const Vec3<T>& other) const {
        return Vec3<T>(x - other.x, y - other.y, z - other.z);
    }

    inline Vec3<T> operator *(float f) const {
        return Vec2<T>(x * f, y * f, z * f);
    }

    inline T operator ^(const Vec3<T>& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    template <typename> friend std::ostream& operator<<(std::ostream& stream, Vec3<T>& v);
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

template <typename T> std::ostream& operator<<(std::ostream& stream, Vec2<T>& v) {
    stream << "(" << v.x << ", " << v.y << ")\n";
    return stream;
}

template <typename T> std::ostream& operator<<(std::ostream& stream, Vec3<T>& v) {
    stream << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
    return stream;
}

#endif