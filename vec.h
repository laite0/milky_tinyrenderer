#ifndef __VEC_H__
#define __VEC_H__

#include <cmath>
#include <ostream>
#include <concepts>

template <typename T> concept Numeric = std::integral<T> || std::floating_point<T>;

template <Numeric T> struct Vec2 {
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

    inline Vec2 operator +(const Vec2& other) const {
        return Vec2(u + other.u, v + other.v);
    }

    inline Vec2 operator -(const Vec2& other) const {
        return Vec2(u - other.u, v - other.v);
    }

    inline Vec2 operator *(float f) const {
        return Vec2(u * f, v * f);
    }

    template <typename> friend std::ostream& operator<<(std::ostream& stream, Vec2& v);
};

template <Numeric T> struct Vec3 {
    T x;
    T y;
    T z;


    Vec3() : x(), y(), z() {}
    Vec3(T _1, T _2, T _3) : x(_1), y(_2), z(_3) {}

    inline Vec3 operator +(const Vec3& other) const {
        return Vec3(x + other.x, y + other.y, z + other.z);
    }

    inline Vec3 operator -(const Vec3& other) const {
        return Vec3(x - other.x, y - other.y, z - other.z);
    }

    inline Vec3 operator-() const {
        return Vec3(-x, -y, -z);
    }

    inline Vec3 operator *(float f) const {
        return Vec2<T>(x * f, y * f, z * f);
    }

    inline T operator ^(const Vec3& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    inline Vec3 normalize() {
        auto length = sqrt(x * x + y * y + z * z);
        return Vec3(x / length, y / length, z / length);
    }

    inline T dot(const Vec3& other) {
        return x * other.x + y * other.y + z * other.z;
    }

    inline Vec3 cross(const Vec3& other) {
        return Vec3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
            );
    }

    template <typename> friend std::ostream& operator<<(std::ostream& stream, Vec3& v);
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