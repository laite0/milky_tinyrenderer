#ifndef GL_H
#define GL_H
#include <functional>

#include "shader.h"
#include "tgaimage.h"
#include "vec.h"


const TGAColor white = {255, 255, 255, 255}; // BGRA order
const TGAColor green = {0, 255, 0, 255};
const TGAColor red = {0, 0, 255, 255};
const TGAColor blue = {255, 128, 64, 255};
const TGAColor yellow = {0, 200, 255, 255};


inline void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, const TGAColor &color) {
    bool steep = std::abs(by - ay) > std::abs(bx - ax);
    if (steep) {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (bx < ax) {
        std::swap(ax, bx);
        std::swap(ay, by);
    }

    int y = ay;
    int scaled_y_to_int_y = 0;
    bool up = by > ay;
    int k = std::abs(by - ay);
    int dx = bx - ax;
    for (int x = ax; x <= bx; x++) {
        if (steep) {
            framebuffer.set(y, x, color);
        } else {
            framebuffer.set(x, y, color);
        }
        scaled_y_to_int_y += 2 * k;
        int change_y = scaled_y_to_int_y > dx;
        y += change_y * (up ? 1 : -1);
        scaled_y_to_int_y -= 2 * dx * change_y;
    }
}

inline double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return 0.5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

inline Vec3d barycentric_coords_2d(int ax, int ay, int bx, int by, int cx, int cy,
                                                                int x, int y, double all) {
    double a = signed_triangle_area(x, y, bx, by, cx, cy);
    double b = signed_triangle_area(ax, ay, x, y, cx, cy);
    double g = signed_triangle_area(ax, ay, bx, by, x, y);
    return {a / all, b / all, g / all};
}

inline void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, TGAImage &framebuffer,
                     const std::function<const TGAColor(double, double, double)>& color_picker) {
    int xMin = std::min(ax, std::min(bx, cx));
    int xMax = std::max(ax, std::max(bx, cx));
    int yMin = std::min(ay, std::min(by, cy));
    int yMax = std::max(ay, std::max(by, cy));

    double area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    // we removed back face culling because we want it be order independent

#pragma omp parallel for
    for (int x = xMin; x <= xMax; x++) {
        for (int y = yMin; y < yMax; y++) {
            auto [alpha, beta, gamma] = barycentric_coords_2d(ax, ay, bx, by, cx, cy, x, y, area).view();
            if (alpha < 0 || beta < 0 || gamma < 0) {
                continue;
            }
            framebuffer.set(x, y, color_picker(alpha, beta, gamma));
        }
    }
}

inline void triangle_with_z(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz,
                            TGAImage &framebuffer, TGAImage &zbuffer,
                            const std::function<const TGAColor(double, double, double)>& color_picker) {
    int xMin = std::min(ax, std::min(bx, cx));
    int xMax = std::max(ax, std::max(bx, cx));
    int yMin = std::min(ay, std::min(by, cy));
    int yMax = std::max(ay, std::max(by, cy));

    double area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    // we removed back face culling because we want it be order independent

#pragma omp parallel for
    for (int x = xMin; x <= xMax; x++) {
        for (int y = yMin; y < yMax; y++) {
            auto [alpha, beta, gamma] = barycentric_coords_2d(ax, ay, bx, by, cx, cy, x, y, area).view();
            auto z = static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz);
            if (alpha < 0 || beta < 0 || gamma < 0 || zbuffer.get(x, y).r >= z) {
                continue;
            }
            zbuffer.set(x, y, {z});
            framebuffer.set(x, y, color_picker(alpha, beta, gamma));
        }
    }
}

inline void shading_triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz,
                            Model const& model, TGAImage& framebuffer, TGAImage& zbuffer, Shader const& shader,
                            VertexData const& vertex_data) {
    int xMin = std::min(ax, std::min(bx, cx));
    int xMax = std::max(ax, std::max(bx, cx));
    int yMin = std::min(ay, std::min(by, cy));
    int yMax = std::max(ay, std::max(by, cy));

    double area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    // we removed back face culling because we want it be order independent

#pragma omp parallel for
    for (int x = xMin; x <= xMax; x++) {
        for (int y = yMin; y < yMax; y++) {
            auto abg = barycentric_coords_2d(ax, ay, bx, by, cx, cy, x, y, area);
            auto [alpha, beta, gamma] = abg.view();
            auto z = static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz);
            if (alpha < 0 || beta < 0 || gamma < 0 || zbuffer.get(x, y).r >= z) {
                continue;
            }
            auto vals = shader.eval_fragment(model, vertex_data, abg);
            if (vals.keep) {
                zbuffer.set(x, y, {z});
                framebuffer.set(x, y, vals.color);
            }
        }
    }
}

#endif