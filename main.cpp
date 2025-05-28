#include <cmath>
#include <iostream>
#include <tuple>
#include <cassert>
#include "tgaimage.h"
#include "model.h"
#include <functional>

#include "matrix.h"

const TGAColor white = {255, 255, 255, 255}; // BGRA order
const TGAColor green = {0, 255, 0, 255};
const TGAColor red = {0, 0, 255, 255};
const TGAColor blue = {255, 128, 64, 255};
const TGAColor yellow = {0, 200, 255, 255};

Vec3f m2v(Matrix m) {
    return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

Matrix v2m(Vec3f v) {
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}

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

void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, TGAImage &framebuffer,
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
            auto [alpha, beta, gamma] = barycentric_coords_2d(ax, ay, bx, by, cx, cy, x, y, area);
            if (alpha < 0 || beta < 0 || gamma < 0) {
                continue;
            }
            framebuffer.set(x, y, color_picker(alpha, beta, gamma));
        }
    }
}

void triangle_with_z(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz,
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
            auto [alpha, beta, gamma] = barycentric_coords_2d(ax, ay, bx, by, cx, cy, x, y, area);
            auto z = static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz);
            if (alpha < 0 || beta < 0 || gamma < 0 || zbuffer.get(x, y).r >= z) {
                continue;
            }
            zbuffer.set(x, y, {z});
            framebuffer.set(x, y, color_picker(alpha, beta, gamma));
        }
    }
}

int model_render(int argc, char **argv) {
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
        std::array<Vec3i, 3> coords;
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model.vertexAt(face[j]);
            coords[i] = Vec3i((v0.x + 1.0) * width / 2.0, (v0.y + 1.0) * height / 2.0, v0.z);
        }

        auto color = TGAColor(rand()%255, rand()%255, rand()%255, 255);
        triangle(coords[0].x, coords[0].y, coords[0].z,
                 coords[1].x, coords[1].y, coords[1].z,
                 coords[2].x, coords[2].y, coords[2].z,
                 framebuffer,
                 [&color] (double alpha, double beta, double gamma) { return color; });
    }

    framebuffer.flip_vertically();

    framebuffer.write_tga_file("model_render.tga");
    return 0;
}

Vec3f transform_to_viewport(Vec3f v, const int width, const int height) {
    return { (v.x + 1.0f) * width/2, (v.y + 1.0f) * height/2, (v.z + 1.0f) * 255/2 };
}

int model_render_in_z(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    constexpr int width = 800;
    constexpr int height = 800;
    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage     zbuffer(width, height, TGAImage::RGB);

    for (int i = 0; i < model.numberOfFaces(); i++) {
        std::vector<int> face = model.faceAt(i);
        std::array<Vec3i, 3> coords;
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model.vertexAt(face[j]);
            auto [x, y, z] = transform_to_viewport(v0, width, height);
            coords[j] = Vec3i(x, y, z);
        }

        auto color = TGAColor(rand()%255, rand()%255, rand()%255, 255);
        triangle_with_z(coords[0].x, coords[0].y, coords[0].z,
                        coords[1].x, coords[1].y, coords[1].z,
                        coords[2].x, coords[2].y, coords[2].z,
                        framebuffer, zbuffer,
                        [&color] (double alpha, double beta, double gamma) { return color; });
    }

    framebuffer.flip_vertically();

    framebuffer.write_tga_file("model_render_z_buffered.tga");

    zbuffer.flip_vertically();

    zbuffer.write_tga_file("model_render_z_buffer.tga");
    return 0;
}

Matrix model_transform() {
    return Matrix::identity(4);
}

Matrix view_transform(const Vec3f target, const Vec3f where, const Vec3f up) {
    //       ^ y
    //       |
    // x  <--X(z)

    auto w = where;
    auto v = (target - where).normalize(); // -z
    auto r = -v.cross(up).normalize(); // x
    auto u = r.cross(v).normalize();  // y
    Matrix moveToOrigin = Matrix::identity(4);
    moveToOrigin[0] = {    1,    0,    0, -w.x};
    moveToOrigin[1] = {    0,    1,    0, -w.y};
    moveToOrigin[2] = {    0,    0,    1, -w.z};
    moveToOrigin[3] = {    0,    0,    0,    1};

    Matrix moveToView = Matrix::identity(4);
    moveToView[0] = {  r.x,  u.x,  v.x,    0};
    moveToView[1] = {  r.y,  u.y,  v.y,    0};
    moveToView[2] = {  r.z,  u.z,  v.z,    0};
    moveToView[3] = {    0,    0,    0,    1};

    const Matrix viewToOrigin = moveToView.transpose(); // inv

    return viewToOrigin * moveToOrigin;
}

Matrix perspective_transform(const float z_near, const float width, const float height) {
    auto two_n = 2 * z_near;

    Matrix perspective = Matrix::identity(4);
    perspective[0] = {two_n / width,               0,    0,      0};
    perspective[1] = {            0,  two_n / height,    0,      0};
    perspective[2] = {            0,               0,    1, -two_n};
    perspective[3] = {            0,               0,    1,      1};
    return perspective;
}

Matrix viewport_transform(int vp_width, int vp_height, int depth) {
    Matrix m = Matrix::identity(4);
    m[0][3] = vp_width/2.f;
    m[1][3] = vp_height/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = vp_width/2.f;
    m[1][1] = vp_height/2.f;
    m[2][2] = depth/2.f;
    return m;
}

Vec3f transform(const Vec3f v, const Matrix &matrix) {
    return m2v(matrix * v2m(v));
}

int model_render_perspective(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    Vec3f camPos = Vec3f(0, 0, 1);
    Vec3f camTowards = Vec3f(0, 0, -1);
    Vec3f camUp = Vec3f(0, 1, 0);
    auto view = view_transform(camTowards, camPos, camUp);
    auto perspective = perspective_transform(-1, 1, 1);


    constexpr int width = 800;
    constexpr int height = 800;

    auto viewport = viewport_transform(width, height, 255);
    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage     zbuffer(width, height, TGAImage::RGB);

    for (int i = 0; i < model.numberOfFaces(); i++) {
        std::vector<int> face = model.faceAt(i);
        std::array<Vec3i, 3> coords;
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model.vertexAt(face[j]);
            auto [x, y, z] = m2v(viewport * perspective * view * v2m(v0));
            coords[j] = Vec3i(x, y, z);
        }

        auto color = TGAColor(rand()%255, rand()%255, rand()%255, 255);
        triangle_with_z(coords[0].x, coords[0].y, coords[0].z,
                        coords[1].x, coords[1].y, coords[1].z,
                        coords[2].x, coords[2].y, coords[2].z,
                        framebuffer, zbuffer,
                        [&color] (double alpha, double beta, double gamma) { return color; });
    }

    framebuffer.flip_vertically();

    framebuffer.write_tga_file("model_render_perspective.tga");

    zbuffer.flip_vertically();

    zbuffer.write_tga_file("model_render_perspective_z_buffer.tga");
    return 0;
}

int grayscale_barycentric_triangle(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax = 17, ay =  4, az =  13;
    int bx = 55, by = 39, bz = 128;
    int cx = 23, cy = 59, cz = 255;

    triangle(ax, ay, az, bx, by, bz, cx, cy, cz, framebuffer,
             [az, bz, cz] (double alpha, double beta, double gamma) {
                 auto z = static_cast<unsigned char>(alpha * az + beta * bz + gamma * cz);
                 return TGAColor(z, z, z, 0);
    });

    framebuffer.write_tga_file("grayscale_barycentric_triangle.tga");
    return 0;
}

int rgb_barycentric_triangle(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax = 17, ay =  4, az =  13;
    int bx = 55, by = 39, bz = 128;
    int cx = 23, cy = 59, cz = 255;

    triangle(ax, ay, az, bx, by, bz, cx, cy, cz, framebuffer,
             [] (double alpha, double beta, double gamma) { return TGAColor(alpha * 255, beta * 255, gamma * 255, 0); });

    framebuffer.write_tga_file("rgb_barycentric_triangle.tga");
    return 0;
}

int basic_render(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax =  7, ay =  3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    framebuffer.set(ax, ay, white);
    framebuffer.set(bx, by, white);
    framebuffer.set(cx, cy, white);

    framebuffer.write_tga_file("basic_render.tga");
    return 0;
}
int main(int argc, char **argv) {
    return model_render_perspective(argc, argv);
}