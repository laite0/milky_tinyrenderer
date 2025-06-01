#ifndef SHADER_H
#define SHADER_H

#include "model.h"
#include "vec.h"

static constexpr size_t UNIFORM_LIGHT_DIR = 0;

static constexpr size_t UNIFORMS = UNIFORM_LIGHT_DIR + 1;

static constexpr size_t VARYING_INTENSITY = 0;

static constexpr size_t VARYINGS = VARYING_INTENSITY + 1;

struct VertexData {
    std::array<Vec3f, VARYINGS> data;
};

struct FragementData {
    TGAColor color;
    bool keep;
};

struct UniformData {
    std::array<Vec3f const, UNIFORMS> const& data;
};

class Shader {
    public:
    Shader();
    virtual ~Shader();

    [[nodiscard]] virtual Vec4f const eval_vertex(Model const& model, UniformData const& uniforms,
                                                   int iface, int nth_vert, VertexData& vertex_data) const = 0;

    [[nodiscard]] virtual FragementData const eval_fragment(UniformData const& uniforms, VertexData const& vertex_data,
                                                             Vec3d const& alphabetagamma) const = 0;
};

inline Shader::Shader() = default;
inline Shader::~Shader() {}



#endif //SHADER_H
