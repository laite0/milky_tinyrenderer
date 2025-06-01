#include "shaders.h"
#include "tgaimage.h"
#include "utils.h"

GouraudShader::GouraudShader() = default;
GouraudShader::~GouraudShader() = default;

[[nodiscard]] Vec4f const GouraudShader::eval_vertex(Model const & model, UniformData const & uniforms,
                                      int iface, int nth_vert, VertexData & out_vertex_data) const {
    out_vertex_data.data[VARYING_INTENSITY][nth_vert] = std::max(0.f, model.normal_at(iface, nth_vert).dot(uniforms.data[UNIFORM_LIGHT_DIR])); // get diffuse lighting intensity
    Vec4f gl_Vertex = into_homo(model.vertex_at(iface, nth_vert)); // read the vertex from .obj file
    return gl_Vertex;
}

[[nodiscard]] FragementData const GouraudShader::eval_fragment(UniformData const& uniforms, VertexData const& vertex_data,
                                                               Vec3d const& alphabetagamma) const {
    float intensity = vertex_data.data[VARYING_INTENSITY].dot(Vec3f(alphabetagamma.x, alphabetagamma.y, alphabetagamma.z));   // interpolate intensity for the current pixel
    return {
        TGAColor(255, 255, 255) * intensity, true
    };
}

[[nodiscard]] Vec4f const ToonShader::eval_vertex(Model const & model, UniformData const & uniforms,
                                      int iface, int nth_vert, VertexData & out_vertex_data) const {
    out_vertex_data.data[VARYING_INTENSITY][nth_vert] = std::max(0.f, model.normal_at(iface, nth_vert).dot(uniforms.data[UNIFORM_LIGHT_DIR])); // get diffuse lighting intensity
    Vec4f gl_Vertex = into_homo(model.vertex_at(iface, nth_vert)); // read the vertex from .obj file
    return gl_Vertex;
}

ToonShader::ToonShader() = default;
ToonShader::~ToonShader() = default;

[[nodiscard]] FragementData const ToonShader::eval_fragment(UniformData const& uniforms, VertexData const& vertex_data,
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


