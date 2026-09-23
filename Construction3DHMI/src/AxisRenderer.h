#pragma once

#include <glad/gl.h>

class AxisRenderer
{
public:
    explicit AxisRenderer(
        float length
    );

    ~AxisRenderer();

    AxisRenderer(const AxisRenderer&) = delete;
    AxisRenderer& operator=(const AxisRenderer&) = delete;

    void Draw(
        unsigned int shaderProgram
    ) const;

private:
    unsigned int VAO_ = 0;
    unsigned int VBO_ = 0;
};
