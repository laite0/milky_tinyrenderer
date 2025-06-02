#include <iostream>

#include "shaders.h"
#include "tgaimage.h"
#include "utils.h"

GouraudShader::GouraudShader() = default;
GouraudShader::~GouraudShader() = default;

[[nodiscard]] Vec4f const GouraudShader::eval_vertex(Model const & model,
                                      int iface, int nth_vert, VertexData & out_vertex_data) const {
    out_vertex_data.data[VARYING_INTENSITY][nth_vert] = std::max(0.f, model.normal_at(iface, nth_vert).dot(lightDirection)); // get diffuse lighting intensity
    Vec4f gl_Vertex = into_homo(model.vertex_at(iface, nth_vert)); // read the vertex from .obj file
    return gl_Vertex;
}

[[nodiscard]] FragementData const GouraudShader::eval_fragment(Model const & model, VertexData const& vertex_data,
                                                               Vec3d const& alphabetagamma) const {
    float intensity = vertex_data.data[VARYING_INTENSITY].dot(Vec3f(alphabetagamma.x, alphabetagamma.y, alphabetagamma.z));   // interpolate intensity for the current pixel
    return {
        TGAColor(255, 255, 255) * intensity, true
    };
}

ToonShader::ToonShader() = default;
ToonShader::~ToonShader() = default;

[[nodiscard]] Vec4f const ToonShader::eval_vertex(Model const & model,
                                      int iface, int nth_vert, VertexData & out_vertex_data) const {
    out_vertex_data.data[VARYING_INTENSITY][nth_vert] = std::max(0.f, model.normal_at(iface, nth_vert).dot(lightDirection)); // get diffuse lighting intensity
    Vec4f gl_Vertex = into_homo(model.vertex_at(iface, nth_vert)); // read the vertex from .obj file
    return gl_Vertex;
}

[[nodiscard]] FragementData const ToonShader::eval_fragment(Model const & model, VertexData const& vertex_data,
                                                               Vec3d const& alphabetagamma) const {
    float intensity = vertex_data.data[VARYING_INTENSITY].dot(Vec3f(alphabetagamma.x, alphabetagamma.y, alphabetagamma.z));   // interpolate intensity for the current pixel
    if (intensity>.85) intensity = 1;
    else if (intensity>.60) intensity = .80;
    else if (intensity>.45) intensity = .60;
    else if (intensity>.30) intensity = .45;
    else if (intensity>.15) intensity = .30;
    else intensity = 0;
    return {
        TGAColor(255, 155, 0) * intensity, true
    };
}

PhongShader::PhongShader() = default;
PhongShader::~PhongShader() = default;

[[nodiscard]] Vec4f const PhongShader::eval_vertex(Model const & model, int iface, int nth_vert, VertexData & out_vertex_data) const {
    auto uv = model.uv_at(iface, nth_vert);
    out_vertex_data.data[PHONG_VARYING_UV1 + nth_vert] = Vec3f(uv.x, uv.y, 0);
    out_vertex_data.data[VARYING_INTENSITY][nth_vert] = std::max(0.f, model.normal_at(iface, nth_vert).dot(lightDirection)); // get diffuse lighting intensity
    Vec4f gl_Vertex = into_homo(model.vertex_at(iface, nth_vert)); // read the vertex from .obj file
    return gl_Vertex;
}

[[nodiscard]] FragementData const PhongShader::eval_fragment(Model const & model, VertexData const& vertex_data,
                                                             Vec3d const& alphabetagamma) const {
    auto v3f = Vec3f(alphabetagamma.x, alphabetagamma.y, alphabetagamma.z);
    Matrix<float, 2, 3> uvs;
    uvs[0] = {vertex_data.data[PHONG_VARYING_UV1].x, vertex_data.data[PHONG_VARYING_UV2].x, vertex_data.data[PHONG_VARYING_UV3].x};
    uvs[1] = {vertex_data.data[PHONG_VARYING_UV1].y, vertex_data.data[PHONG_VARYING_UV2].y, vertex_data.data[PHONG_VARYING_UV3].y};
    Vec2f uv = from_column(uvs * v3f.column_into());
    Vec3f n = transform(model.normal_at(uv), mvp_inv).normalized();
    Vec3f l = transform(lightDirection, mvp).normalized();
    Vec3f v = transform(cam_pos, mvp).normalized();
    Vec3f h = (v + l).normalized();
    // we assume a directional light with constant intensity 1

    float intensity = /* diffuse */std::max(0.f, n.dot(l)) + /*specular*/std::pow(std::max(0.f, n.dot(h)), 50);
    return {
        model.diffuse_at(uv) * intensity, true
    };
}


