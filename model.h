#ifndef MILKY_TINYRENDERER_MODEL_H
#define MILKY_TINYRENDERER_MODEL_H
#include <vector>
#include <string>
#include "vec.h"
#include "tgaimage.h"

class Model {
    std::vector<Vec3f> vertices;
    union Id {
        struct {
            size_t vertex_id;
            size_t uv_id;
            size_t normal_id;
        };
        size_t data[3];
    };
    std::vector<std::vector<Id>> faces;
    std::vector<Vec3f> normals;
    std::vector<Vec2f> uv;
    TGAImage diffusemap;
    TGAImage normalmap;
    TGAImage specularmap;
    void load_texture(const std::string &filename, const char *suffix, TGAImage &img);
public:
    explicit Model(const char *filename);
    ~Model();
    [[nodiscard]] size_t number_of_vertices() const;
    [[nodiscard]] size_t number_of_faces() const;
    [[nodiscard]] Vec3f normal_at(size_t iface, size_t nth_vertex) const;
    [[nodiscard]] Vec3f normal_at(const Vec2f &uv) const;
    [[nodiscard]] Vec3f vertex_at(size_t i) const;
    [[nodiscard]] Vec3f vertex_at(size_t iface, size_t nth_vertex) const;
    [[nodiscard]] Vec2f uv_at(size_t iface, size_t nth_vertex) const;
    [[nodiscard]] TGAColor diffuse_at(const Vec2f &uv) const;
    [[nodiscard]] float specular_at(const Vec2f &uv) const;
    [[nodiscard]] std::vector<size_t> face_at(size_t idx) const;
};
#endif