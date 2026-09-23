#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Mesh.h"
#include "Material.h"


struct aiNode;
struct aiScene;
struct aiMesh;
struct aiMaterial;


class Model
{
public:
    explicit Model(
        const std::string& path
    );

    void Draw(
        unsigned int shaderProgram
    ) const;


    const glm::vec3& getBoundsMin() const;
    const glm::vec3& getBoundsMax() const;

    glm::vec3 getCenter() const;

    float getRadius() const;


private:
    std::vector<Mesh> meshes_;

    glm::vec3 boundsMin_;
    glm::vec3 boundsMax_;

    bool boundsInitialized_ = false;


    void loadModel(
        const std::string& path
    );


    void processNode(
        aiNode* node,
        const aiScene* scene,
        const glm::mat4& parentTransform
    );


    Mesh processMesh(
        aiMesh* mesh,
        const aiScene* scene,
        const glm::mat4& transform
    );


    Material processMaterial(
        aiMaterial* material
    );


    void updateBounds(
        const glm::vec3& position
    );
};
