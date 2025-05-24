#include <cmath>
#include <iostream>
#include <tuple>
#include <cassert>
#include "tgaimage.h"
#include "model.h"

const TGAColor white = {255, 255, 255, 255}; // BGRA order
const TGAColor green = {0, 255, 0, 255};
const TGAColor red = {0, 0, 255, 255};
const TGAColor blue = {255, 128, 64, 255};
const TGAColor yellow = {0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage &framebuffer, const TGAColor &color) {
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

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return 0.5 * ((by - ay) * (bx + ax) + (cy - by) * (cx + bx) + (ay - cy) * (ax + cx));
}

std::tuple<double, double, double> barycentric_coords_2d(int ax, int ay, int bx, int by, int cx, int cy, int x, int y, double all) {
    double a = signed_triangle_area(x, y, bx, by, cx, cy);
    double b = signed_triangle_area(ax, ay, x, y, cx, cy);
    double g = signed_triangle_area(ax, ay, bx, by, x, y);
    return {a / all, b / all, g / all};
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, const TGAColor& color) {
    int xMin = std::min(ax, std::min(bx, cx));
    int xMax = std::max(ax, std::max(bx, cx));
    int yMin = std::min(ay, std::min(by, cy));
    int yMax = std::max(ay, std::max(by, cy));

    double area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    // we removed back face culling because we want it be order independent

#pragma omp parallel for
    for (int x = xMin; x <= xMax; x++) {
        for (int y = yMin; y < yMax; y++) {
            auto [alpha, beta, gamma] = barycentric_coords_2d(ax, ay, bx, by, cx, cy, x, y, area);
            if (alpha < 0 || beta < 0 || gamma < 0) {
                continue;
            }
            framebuffer.set(x, y, color);
        }
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    constexpr int width = 800;
    constexpr int height = 800;
    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);


    for (int i = 0; i < model.numberOfFaces(); i++) {
        std::vector<int> face = model.faceAt(i);
        Vec3<Vec2i> coords;
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model.vertexAt(face[j]);
            coords.raw[j] = Vec2i((v0.x + 1.0) * width / 2.0, (v0.y + 1.0) * height / 2.0);
        }
        auto x = coords.x;
        triangle(coords.x.x, coords.x.y, coords.y.x, coords.y.y, coords.z.x, coords.z.y, framebuffer,
                 TGAColor(rand()%255, rand()%255, rand()%255, 255));
    }

    framebuffer.flip_vertically();

    framebuffer.write_tga_file("wire_frame_attempt.tga");
    return 0;
}