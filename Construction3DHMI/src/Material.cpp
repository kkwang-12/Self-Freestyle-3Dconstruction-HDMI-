
#include <vector>

#include <glad/gl.h>
#include <glm/glm.hpp>

#include "Material.h"


struct Vertex
{
    glm::vec3 Position{0.0f};
    glm::vec3 Normal{0.0f, 1.0f, 0.0f};
};


class Mesh
{
public:
    Mesh(
        const std::vector<Vertex>& vertices,
        const std::vector<unsigned int>& indices,
        const Material& material
    );

    ~Mesh();

    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void Draw(
        unsigned int shaderProgram
    ) const;


private:
    std::vector<Vertex> vertices_;
    std::vector<unsigned int> indices_;

    Material material_;

    unsigned int VAO_ = 0;
    unsigned int VBO_ = 0;
    unsigned int EBO_ = 0;

    void setupMesh();
    void release();
};
