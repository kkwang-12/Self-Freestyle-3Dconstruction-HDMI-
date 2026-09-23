#include "GridRenderer.h"

#include <vector>


GridRenderer::GridRenderer(
    UpAxis upAxis,
    float size,
    int divisions
)
{
    buildGrid(
        upAxis,
        size,
        divisions
    );
}


GridRenderer::~GridRenderer()
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


void GridRenderer::buildGrid(
    UpAxis upAxis,
    float size,
    int divisions
)
{
    std::vector<glm::vec3> vertices;

    if (divisions < 1)
    {
        divisions = 1;
    }

    const float step =
        (size * 2.0f) /
        static_cast<float>(divisions);

    for (int i = 0; i <= divisions; ++i)
    {
        const float p =
            -size +
            static_cast<float>(i) * step;

        if (upAxis == UpAxis::Z)
        {
            // XY ground plane

            vertices.emplace_back(
                -size,
                p,
                0.0f
            );

            vertices.emplace_back(
                size,
                p,
                0.0f
            );

            vertices.emplace_back(
                p,
                -size,
                0.0f
            );

            vertices.emplace_back(
                p,
                size,
                0.0f
            );
        }
        else
        {
            // XZ ground plane

            vertices.emplace_back(
                -size,
                0.0f,
                p
            );

            vertices.emplace_back(
                size,
                0.0f,
                p
            );

            vertices.emplace_back(
                p,
                0.0f,
                -size
            );

            vertices.emplace_back(
                p,
                0.0f,
                size
            );
        }
    }

    vertexCount_ =
        static_cast<GLsizei>(
            vertices.size()
        );

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
        static_cast<GLsizeiptr>(
            vertices.size() *
            sizeof(glm::vec3)
        ),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(glm::vec3),
        nullptr
    );

    glBindVertexArray(0);
}


void GridRenderer::Draw(
    unsigned int shaderProgram
) const
{
    const int colorLocation =
        glGetUniformLocation(
            shaderProgram,
            "lineColor"
        );

    glUniform3f(
        colorLocation,
        0.30f,
        0.32f,
        0.35f
    );

    glBindVertexArray(
        VAO_
    );

    glDrawArrays(
        GL_LINES,
        0,
        vertexCount_
    );

    glBindVertexArray(0);
}
