#ifndef UTILS_H
#define UTILS_H

#include "matrix.h"
#include "vec.h"

inline Vec4f into_homo(Vec3f const& v) {
    return {v.x,v.y,v.z,1.f};
}

inline Vec3f from_homo(Vec4f const& v) {
    return {v.x / v.w, v.y / v.w, v.z / v.w};
}

template <size_t N> Vec<float, N> from_column(VecMf<N> const& v) {
    Vec<float, N> ret{};
    for (size_t i = 0; i < N; i++) {
        ret[i] = v[i][0];
    }
    return ret;
}

inline Mat4x4f model_transform() {
    return Mat4x4f::identity<4>();
}

inline Mat4x4f view_transform(Vec3f const& target, Vec3f const& where, Vec3f const& up) {
    //       ^ y
    //       |
    // x  <--X(z)

    Vec3f const& w = where;
    auto v = (target - where).normalized(); // -z
    auto r = -v.cross(up).normalized(); // x
    auto u = r.cross(v).normalized();  // y
    Mat4x4f moveToOrigin = Mat4x4f({{
        {    1,    0,    0, -w.x},
        {    0,    1,    0, -w.y},
        {    0,    0,    1, -w.z},
        {    0,    0,    0,    1}
    }});

    Mat4x4f moveToView = Mat4x4f({{
        {  r.x,  u.x,  v.x,    0},
        {  r.y,  u.y,  v.y,    0},
        {  r.z,  u.z,  v.z,    0},
        {    0,    0,    0,    1}
    }});

    const Mat4x4f viewToOrigin = moveToView.transpose(); // inv

    return viewToOrigin * moveToOrigin;
}

inline Mat4x4f perspective_transform(const float z_near, const float width, const float height) {
    auto two_n = 2 * z_near;

    Mat4x4f perspective = Mat4x4f({{
        {two_n / width,               0,    0,      0},
        {            0,  two_n / height,    0,      0},
        {            0,               0,    1, -two_n},
        {            0,               0,    1,      1}
    }});
    return perspective;
}

inline Mat4x4f viewport_transform(int vp_width, int vp_height, int depth) {
    Mat4x4f m = Mat4x4f({{
        {vp_width/2.f,             0,          0,  vp_width/2.f},
        {           0, vp_height/2.f,          0, vp_height/2.f},
        {           0,             0,  depth/2.f,     depth/2.f},
        {           0,             0,          0,             1}
    }});
    return m;
}

inline Vec3f transform(const Vec3f& v, const Mat4x4f& matrix) {
    return from_homo(from_column(matrix * into_homo(v).column_into()));
}

inline Vec3f transform(const Vec4f& v, const Mat4x4f& matrix) {
    return from_homo(from_column(matrix * v.column_into()));
}

#endif
