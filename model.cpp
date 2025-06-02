#include <iostream>
#include <fstream>
#include <sstream>
#include "model.h"

Model::Model(const char *filename) {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            vertices.push_back(v);
        } else if (!line.compare(0, 3, "vn ")) {
            iss >> trash >> trash;
            Vec3f n;
            for (int i=0;i<3;i++) iss >> n[i];
            normals.push_back(n);
        } else if (!line.compare(0, 3, "vt ")) {
            iss >> trash >> trash;
            Vec2f uv;
            for (int i=0;i<2;i++) iss >> uv[i];
            this->uv.push_back(uv);
        }  else if (!line.compare(0, 2, "f ")) {
            std::vector<Id> f;
            Id tmp{};
            iss >> trash;
            while (iss >> tmp.vertex_id >> trash >> tmp.uv_id >> trash >> tmp.normal_id) {
                for (size_t & i : tmp.data) i--; // in wavefront obj all indices start at 1, not zero
                f.push_back(tmp);
            }
            faces.push_back(f);
        }
    }
    std::cerr << "# v# " << vertices.size() << " f# "  << faces.size() << " vt# " << uv.size() << " vn# " << normals.size() << std::endl;
    load_texture(filename, "_diffuse.tga", diffusemap);
    load_texture(filename, "_nm.tga",      normalmap);
    load_texture(filename, "_spec.tga",    specularmap);
}

Model::~Model() = default;

void Model::load_texture(const std::string &filename, const char *suffix, TGAImage &img) {
    std::string texfile(filename);
    size_t dot = texfile.find_last_of(".");
    if (dot!=std::string::npos) {
        texfile = texfile.substr(0,dot) + std::string(suffix);
        std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") << std::endl;
        img.flip_vertically();
    }
}

size_t Model::number_of_vertices() const {
    return vertices.size();
}

size_t Model::number_of_faces() const {
    return faces.size();
}

std::vector<size_t> Model::face_at(size_t idx) const {
    std::vector<size_t> face;
    for (auto i : faces[idx]) face.push_back(i.vertex_id);
    return face;
}

Vec3f Model::vertex_at(size_t i) const {
    return vertices[i];
}

Vec3f Model::vertex_at(size_t iface, size_t nth_vertex) const {
    return vertices[faces[iface][nth_vertex].vertex_id];
}

TGAColor Model::diffuse_at(const Vec2f &uvf) const {
    Vec2i uv(uvf.x * diffusemap.get_width(), uvf.y * diffusemap.get_height());
    return diffusemap.get(uv.x, uv.y);
}

Vec3f Model::normal_at(const Vec2f &uvf) const {
    Vec2i uv(uvf.x *normalmap.get_width(), uvf.y * normalmap.get_height());
    TGAColor c = normalmap.get(uv.x, uv.y);
    Vec3f res;
    for (int i=0; i<3; i++)
        res[2-i] = static_cast<float>(c.raw[i])/255.f*2.f - 1.f;
    return res;
}

Vec2f Model::uv_at(size_t iface, size_t nth_vertex) const {
    return uv[faces[iface][nth_vertex].uv_id];
}

float Model::specular_at(const Vec2f &uvf) const {
    Vec2i uv(static_cast<int>(uvf.x) * specularmap.get_width(), static_cast<int>(uvf.y) * specularmap.get_height());
    return specularmap.get(uv.x, uv.y).raw[0];
}

Vec3f Model::normal_at(size_t iface, size_t nth_vertex) const {
    size_t idx = faces[iface][nth_vertex].normal_id;
    return normals[idx].normalized();
}