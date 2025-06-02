#include <cmath>
#include <iostream>
#include <cassert>
#include "tgaimage.h"
#include "model.h"
#include <functional>
#include <utils.h>

#include "matrix.h"
#include "shaders.h"
#include "gl.h"


int model_render(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    constexpr int width = 800;
    constexpr int height = 800;
    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);


    for (int i = 0; i < model.number_of_faces(); i++) {
        std::vector<size_t> face = model.face_at(i);
        std::array<Vec3i, 3> coords{};
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model.vertex_at(face[j]);
            auto vx =Vec3i((v0.x + 1.0) * width / 2.0, (v0.y + 1.0) * height / 2.0, v0.z);
            coords[j] = vx;
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

    for (int i = 0; i < model.number_of_faces(); i++) {
        std::vector<size_t> face = model.face_at(i);
        std::array<Vec3i, 3> coords;
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model.vertex_at(face[j]);
            auto [x, y, z] = transform_to_viewport(v0, width, height).view();
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

    auto mvp = viewport * perspective * view;

    for (int i = 0; i < model.number_of_faces(); i++) {
        std::vector<size_t> face = model.face_at(i);
        std::array<Vec3i, 3> coords;
        for (int j = 0; j < 3; j++) {
            Vec3f v0 = model.vertex_at(face[j]);
            // template<size_t C2> Matrix<T, R, C2> operator*(const Matrix<T, C, C2>& a) const;
            auto [x, y, z] = transform(v0, mvp).view();
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

int model_render_perspective_textured(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " obj/model.obj" << std::endl;
        return 1;
    }

    Vec3f camPos = Vec3f(0.5, 0.5, 1);
    Vec3f camTowards = Vec3f(0, 0, -0.5);
    Vec3f camUp = Vec3f(0, 1, 0);
    auto view = view_transform(camTowards, camPos, camUp);
    auto perspective = perspective_transform(-1, 1, 1);


    constexpr int width = 800;
    constexpr int height = 800;

    auto viewport = viewport_transform(width, height, 255);
    Model model(argv[1]);
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage     zbuffer(width, height, TGAImage::RGB);

    auto mvpscr = viewport * perspective * view;
    auto mvp = perspective * view;

    PhongShader shader;
    shader.lightDirection = Vec3f(1, 1, 0);
    shader.mvp = mvp;
    shader.mvp_inv = mvp.inverse().transpose();
    shader.cam_pos = camPos;
    for (int face_id = 0; face_id < model.number_of_faces(); face_id++) {
        std::vector<size_t> face = model.face_at(face_id);
        std::array<Vec3i, 3> coords;
        VertexData vd{
            {Vec3f(1, 1, 1)}
        };
        for (int j = 0; j < 3; j++) {
            auto manu_coord = shader.eval_vertex(model, face_id, j, vd);
            auto [x, y, z] = transform(manu_coord, mvpscr).view();
            coords[j] = Vec3i(x, y, z);
        }

        shading_triangle(coords[0].x, coords[0].y, coords[0].z,
                        coords[1].x, coords[1].y, coords[1].z,
                        coords[2].x, coords[2].y, coords[2].z,
                        model, framebuffer, zbuffer, shader, vd);
    }

    framebuffer.flip_vertically();

    framebuffer.write_tga_file("render_textured_uv_specular.tga");

    zbuffer.flip_vertically();

    zbuffer.write_tga_file("render_textured_z_uv_specular.tga");
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
    return model_render_perspective_textured(argc, argv);
}