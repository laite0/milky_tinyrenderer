#ifndef SHADER_H
#define SHADER_H

#include "model.h"
#include "vec.h"

static constexpr size_t VARYING_INTENSITY = 0;

static constexpr size_t PHONG_VARYING_UV1 = 1;
static constexpr size_t PHONG_VARYING_UV2 = 2;
static constexpr size_t PHONG_VARYING_UV3 = 3;

static constexpr size_t VARYINGS = PHONG_VARYING_UV3 + 1;

struct VertexData {
    std::array<Vec3f, VARYINGS> data;
};

struct FragementData {
    TGAColor color;
    bool keep;
};

class Shader {
    public:
    Vec3f lightDirection;

    Shader();
    virtual ~Shader();

    [[nodiscard]] virtual Vec4f const eval_vertex(Model const& model, int iface, int nth_vert, VertexData& vertex_data) const = 0;

    [[nodiscard]] virtual FragementData const eval_fragment(Model const & model, VertexData const& vertex_data,
                                                            Vec3d const& alphabetagamma) const = 0;
};

inline Shader::Shader() = default;
inline Shader::~Shader() {}



#endif //SHADER_H
