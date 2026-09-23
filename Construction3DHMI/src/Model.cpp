#include "Model.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>

#include <assimp/Importer.hpp>
#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <glm/gtc/matrix_inverse.hpp>


namespace
{

glm::mat4 aiMatrixToGlm(
    const aiMatrix4x4& m
)
{
    glm::mat4 result;

    result[0][0] = m.a1;
    result[1][0] = m.a2;
    result[2][0] = m.a3;
    result[3][0] = m.a4;

    result[0][1] = m.b1;
    result[1][1] = m.b2;
    result[2][1] = m.b3;
    result[3][1] = m.b4;

    result[0][2] = m.c1;
    result[1][2] = m.c2;
    result[2][2] = m.c3;
    result[3][2] = m.c4;

    result[0][3] = m.d1;
    result[1][3] = m.d2;
    result[2][3] = m.d3;
    result[3][3] = m.d4;

    return result;
}

}


Model::Model(
    const std::string& path
)
{
    loadModel(path);
}


void Model::loadModel(
    const std::string& path
)
{
    Assimp::Importer importer;


    const aiScene* scene =
        importer.ReadFile(
            path,

            aiProcess_Triangulate |
            aiProcess_GenSmoothNormals |
            aiProcess_JoinIdenticalVertices
        );


    if (
        scene == nullptr ||
        scene->mRootNode == nullptr ||
        (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
    )
    {
        throw std::runtime_error(
            std::string(
                "Assimp load failed: "
            ) +
            importer.GetErrorString()
        );
    }


    processNode(
        scene->mRootNode,
        scene,
        glm::mat4(1.0f)
    );


    if (!boundsInitialized_)
    {
        throw std::runtime_error(
            "Model contains no valid vertices."
        );
    }


    std::cout
        << "Loaded meshes: "
        << meshes_.size()
        << std::endl;
}


void Model::processNode(
    aiNode* node,
    const aiScene* scene,
    const glm::mat4& parentTransform
)
{
    glm::mat4 localTransform =
        aiMatrixToGlm(
            node->mTransformation
        );


    glm::mat4 globalTransform =
        parentTransform *
        localTransform;


    for (
        unsigned int i = 0;
        i < node->mNumMeshes;
        ++i
    )
    {
        aiMesh* mesh =
            scene->mMeshes[
                node->mMeshes[i]
            ];


        meshes_.push_back(
            processMesh(
                mesh,
                scene,
                globalTransform
            )
        );
    }


    for (
        unsigned int i = 0;
        i < node->mNumChildren;
        ++i
    )
    {
        processNode(
            node->mChildren[i],
            scene,
            globalTransform
        );
    }
}


Mesh Model::processMesh(
    aiMesh* mesh,
    const aiScene* scene,
    const glm::mat4& transform
)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;


    vertices.reserve(
        mesh->mNumVertices
    );


    glm::mat3 normalMatrix =
        glm::inverseTranspose(
            glm::mat3(transform)
        );


    // ========================================================
    // Vertex
    // ========================================================

    for (
        unsigned int i = 0;
        i < mesh->mNumVertices;
        ++i
    )
    {
        Vertex vertex;


        glm::vec4 localPosition(
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z,
            1.0f
        );


        glm::vec4 worldPosition =
            transform *
            localPosition;


        vertex.Position =
            glm::vec3(
                worldPosition
            );


        if (mesh->HasNormals())
        {
            glm::vec3 localNormal(
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            );


            vertex.Normal =
                glm::normalize(
                    normalMatrix *
                    localNormal
                );
        }


        updateBounds(
            vertex.Position
        );


        vertices.push_back(
            vertex
        );
    }


    // ========================================================
    // Index
    // ========================================================

    for (
        unsigned int i = 0;
        i < mesh->mNumFaces;
        ++i
    )
    {
        const aiFace& face =
            mesh->mFaces[i];


        for (
            unsigned int j = 0;
            j < face.mNumIndices;
            ++j
        )
        {
            indices.push_back(
                face.mIndices[j]
            );
        }
    }


    // ========================================================
    // Material
    // ========================================================

    Material material;


    if (
        mesh->mMaterialIndex <
        scene->mNumMaterials
    )
    {
        aiMaterial* aiMat =
            scene->mMaterials[
                mesh->mMaterialIndex
            ];


        material =
            processMaterial(
                aiMat
            );
    }


    return Mesh(
        vertices,
        indices,
        material
    );
}


Material Model::processMaterial(
    aiMaterial* material
)
{
    Material result;


    // ========================================================
    // Base Color
    //
    // GLB/glTF 优先读取 PBR BaseColor。
    // 如果没有，再退回传统 Diffuse。
    // ========================================================

    aiColor4D color;


    if (
        material->Get(
            AI_MATKEY_BASE_COLOR,
            color
        )
        ==
        AI_SUCCESS
    )
    {
        result.baseColor =
            glm::vec4(
                color.r,
                color.g,
                color.b,
                color.a
            );
    }
    else if (
        material->Get(
            AI_MATKEY_COLOR_DIFFUSE,
            color
        )
        ==
        AI_SUCCESS
    )
    {
        result.baseColor =
            glm::vec4(
                color.r,
                color.g,
                color.b,
                color.a
            );
    }


    // ========================================================
    // Metallic
    // ========================================================

    float metallic = 0.0f;


    if (
        material->Get(
            AI_MATKEY_METALLIC_FACTOR,
            metallic
        )
        ==
        AI_SUCCESS
    )
    {
        result.metallic =
            metallic;
    }


    // ========================================================
    // Roughness
    // ========================================================

    float roughness = 0.5f;


    if (
        material->Get(
            AI_MATKEY_ROUGHNESS_FACTOR,
            roughness
        )
        ==
        AI_SUCCESS
    )
    {
        result.roughness =
            roughness;
    }


    // 暂时打印出来，方便我们验证 CET200 的 GLB
    std::cout
        << "Material: baseColor=("
        << result.baseColor.r << ", "
        << result.baseColor.g << ", "
        << result.baseColor.b << ", "
        << result.baseColor.a << ")"
        << " metallic="
        << result.metallic
        << " roughness="
        << result.roughness
        << std::endl;


    return result;
}


void Model::updateBounds(
    const glm::vec3& position
)
{
    if (!boundsInitialized_)
    {
        boundsMin_ = position;
        boundsMax_ = position;

        boundsInitialized_ = true;

        return;
    }


    boundsMin_ =
        glm::min(
            boundsMin_,
            position
        );


    boundsMax_ =
        glm::max(
            boundsMax_,
            position
        );
}


void Model::Draw(
    unsigned int shaderProgram
) const
{
    for (
        const auto& mesh :
        meshes_
    )
    {
        mesh.Draw(
            shaderProgram
        );
    }
}


const glm::vec3&
Model::getBoundsMin() const
{
    return boundsMin_;
}


const glm::vec3&
Model::getBoundsMax() const
{
    return boundsMax_;
}


glm::vec3
Model::getCenter() const
{
    return
        (
            boundsMin_ +
            boundsMax_
        )
        *
        0.5f;
}


float
Model::getRadius() const
{
    glm::vec3 halfExtent =
        (
            boundsMax_ -
            boundsMin_
        )
        *
        0.5f;


    return glm::length(
        halfExtent
    );
}
