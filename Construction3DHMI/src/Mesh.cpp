#include "Mesh.h"

#include <cstddef>
#include <utility>


Mesh::Mesh(
    const std::vector<Vertex>& vertices,
    const std::vector<unsigned int>& indices,
    const Material& material
)
    : vertices_(vertices),
      indices_(indices),
      material_(material)
{
    setupMesh();
}


Mesh::~Mesh()
{
    release();
}


Mesh::Mesh(Mesh&& other) noexcept
{
    vertices_ = std::move(other.vertices_);
    indices_ = std::move(other.indices_);

    material_ = other.material_;

    VAO_ = other.VAO_;
    VBO_ = other.VBO_;
    EBO_ = other.EBO_;

    other.VAO_ = 0;
    other.VBO_ = 0;
    other.EBO_ = 0;
}


Mesh& Mesh::operator=(Mesh&& other) noexcept
{
    if (this != &other)
    {
        release();

        vertices_ = std::move(other.vertices_);
        indices_ = std::move(other.indices_);

        material_ = other.material_;

        VAO_ = other.VAO_;
        VBO_ = other.VBO_;
        EBO_ = other.EBO_;

        other.VAO_ = 0;
        other.VBO_ = 0;
        other.EBO_ = 0;
    }

    return *this;
}


void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO_);
    glGenBuffers(1, &VBO_);
    glGenBuffers(1, &EBO_);

    glBindVertexArray(VAO_);


    // ========================================================
    // Vertex Buffer
    // ========================================================

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO_
    );

    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(
            vertices_.size() * sizeof(Vertex)
        ),
        vertices_.data(),
        GL_STATIC_DRAW
    );


    // ========================================================
    // Index Buffer
    // ========================================================

    glBindBuffer(
        GL_ELEMENT_ARRAY_BUFFER,
        EBO_
    );

    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(
            indices_.size() * sizeof(unsigned int)
        ),
        indices_.data(),
        GL_STATIC_DRAW
    );


    // ========================================================
    // Position
    // ========================================================

    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(
            offsetof(Vertex, Position)
        )
    );


    // ========================================================
    // Normal
    // ========================================================

    glEnableVertexAttribArray(1);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<void*>(
            offsetof(Vertex, Normal)
        )
    );


    glBindVertexArray(0);
}


void Mesh::Draw(
    unsigned int shaderProgram
) const
{
    // ========================================================
    // Material uniforms
    // ========================================================

    int baseColorLocation =
        glGetUniformLocation(
            shaderProgram,
            "materialBaseColor"
        );

    int metallicLocation =
        glGetUniformLocation(
            shaderProgram,
            "materialMetallic"
        );

    int roughnessLocation =
        glGetUniformLocation(
            shaderProgram,
            "materialRoughness"
        );


    glUniform4f(
        baseColorLocation,
        material_.baseColor.r,
        material_.baseColor.g,
        material_.baseColor.b,
        material_.baseColor.a
    );


    glUniform1f(
        metallicLocation,
        material_.metallic
    );


    glUniform1f(
        roughnessLocation,
        material_.roughness
    );


    // ========================================================
    // Draw
    // ========================================================

    glBindVertexArray(VAO_);

    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(
            indices_.size()
        ),
        GL_UNSIGNED_INT,
        nullptr
    );

    glBindVertexArray(0);
}


void Mesh::release()
{
    if (EBO_ != 0)
    {
        glDeleteBuffers(
            1,
            &EBO_
        );

        EBO_ = 0;
    }


    if (VBO_ != 0)
    {
        glDeleteBuffers(
            1,
            &VBO_
        );

        VBO_ = 0;
    }


    if (VAO_ != 0)
    {
        glDeleteVertexArrays(
            1,
            &VAO_
        );

        VAO_ = 0;
    }
}
