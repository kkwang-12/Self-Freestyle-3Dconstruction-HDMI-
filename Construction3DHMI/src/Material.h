#pragma once

#include <glm/glm.hpp>

struct Material
{
    // glTF / GLB Base Color
    glm::vec4 baseColor{
        0.8f,
        0.8f,
        0.8f,
        1.0f
    };

    // 为后续 PBR 保留
    float metallic = 0.0f;
    float roughness = 0.5f;
};
