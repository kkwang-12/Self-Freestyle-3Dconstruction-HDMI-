#pragma once

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "OrbitCamera.h"

class GridRenderer
{
public:
    GridRenderer(
        UpAxis upAxis,
        float size,
        int divisions
    );

    ~GridRenderer();

    GridRenderer(const GridRenderer&) = delete;
    GridRenderer& operator=(const GridRenderer&) = delete;

    void Draw(
        unsigned int shaderProgram
    ) const;

private:
    unsigned int VAO_ = 0;
    unsigned int VBO_ = 0;

    GLsizei vertexCount_ = 0;

    void buildGrid(
        UpAxis upAxis,
        float size,
        int divisions
    );
};
