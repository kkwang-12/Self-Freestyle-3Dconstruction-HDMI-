#pragma once


#include <string>
#include <vector>

#include <glm/glm.hpp>


class Mesh;


class SceneNode
{

public:

    explicit SceneNode(
        const std::string& name
    );


    ~SceneNode();



    void addChild(
        SceneNode* child
    );



    void setMesh(
        Mesh* mesh
    );



    void setTransform(
        const glm::mat4& transform
    );



    void rotate(
        float angle,
        const glm::vec3& axis
    );



    void draw(
        unsigned int shader,
        const glm::mat4& parentTransform
    );



    const std::string& getName() const;



private:


    std::string name_;


    glm::mat4 localTransform_{
        1.0f
    };


    Mesh* mesh_ = nullptr;



    std::vector<SceneNode*> children_;

};
