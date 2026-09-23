#include "AxisRenderer.h"
#include <cstddef>
#include <glm/glm.hpp>


struct AxisVertex
{
    glm::vec3 position;
    glm::vec3 color;
};


AxisRenderer::AxisRenderer(
    float length
)
{
    const AxisVertex vertices[] =
    {
        // X = Red
        {
            {0.0f, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f}
        },
        {
            {length, 0.0f, 0.0f},
            {1.0f, 0.0f, 0.0f}
        },

        // Y = Green
        {
            {0.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f}
        },
        {
            {0.0f, length, 0.0f},
            {0.0f, 1.0f, 0.0f}
        },

        // Z = Blue
        {
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 1.0f}
        },
        {
            {0.0f, 0.0f, length},
            {0.0f, 0.0f, 1.0f}
        }
    };


    glGenVertexArrays(
        1,
        &VAO_
    );

    glGenBuffers(
        1,
        &VBO_
    );

    glBindVertexArray(
        VAO_
    );

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO_
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );


    // Position
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(AxisVertex),
        reinterpret_cast<void*>(
            offsetof(
                AxisVertex,
                position
            )
        )
    );


    // Color
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(AxisVertex),
        reinterpret_cast<void*>(
            offsetof(
                AxisVertex,
                color
            )
        )
    );


    glBindVertexArray(0);
}


AxisRenderer::~AxisRenderer()
{
    if (VBO_ != 0)
    {
        glDeleteBuffers(
            1,
            &VBO_
        );
    }

    if (VAO_ != 0)
    {
        glDeleteVertexArrays(
            1,
            &VAO_
        );
    }
}


void AxisRenderer::Draw(
    unsigned int shaderProgram
) const
{
    (void)shaderProgram;

    glBindVertexArray(
        VAO_
    );

    glLineWidth(2.0f);

    glDrawArrays(
        GL_LINES,
        0,
        6
    );

    glLineWidth(1.0f);

    glBindVertexArray(0);
}
