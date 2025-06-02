//
// Created by CMB-00003-B on 2025/6/1.
//

#ifndef SHADERS_H
#define SHADERS_H
#include "shader.h"

struct GouraudShader : public Shader {
    GouraudShader();
    ~GouraudShader() override;
    FragementData const eval_fragment(Model const & model, VertexData const &vertex_data, Vec3d const &alphabetagamma) const override;
    Vec4f const eval_vertex(Model const &model, int iface, int nth_vert, VertexData &vertex_data) const override;
};

struct ToonShader : public Shader {
    ToonShader();
    ~ToonShader() override;
    FragementData const eval_fragment(Model const & model, VertexData const &vertex_data, Vec3d const &alphabetagamma) const override;
    Vec4f const eval_vertex(Model const &model, int iface, int nth_vert, VertexData &vertex_data) const override;
};

struct PhongShader : public Shader {
public:
    Mat4x4f mvp;
    Mat4x4f mvp_inv;
    Vec3f cam_pos;

    PhongShader();
    ~PhongShader() override;
    FragementData const eval_fragment(Model const & model, VertexData const &vertex_data, Vec3d const &alphabetagamma) const override;
    Vec4f const eval_vertex(Model const &model, int iface, int nth_vert, VertexData &vertex_data) const override;
};

#endif //SHADERS_H
