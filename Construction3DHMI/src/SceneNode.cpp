#include "SceneNode.h"

#include "Mesh.h"


#include <glm/gtc/matrix_transform.hpp>



SceneNode::SceneNode(
    const std::string& name
)
    :
    name_(name)
{

}



SceneNode::~SceneNode()
{

    for(auto child: children_)
    {
        delete child;
    }

}



void SceneNode::addChild(
    SceneNode* child
)
{
    children_.push_back(
        child
    );
}



void SceneNode::setMesh(
    Mesh* mesh
)
{
    mesh_ = mesh;
}



void SceneNode::setTransform(
    const glm::mat4& transform
)
{
    localTransform_ =
        transform;
}



void SceneNode::rotate(
    float angle,
    const glm::vec3& axis
)
{

    localTransform_ =
        glm::rotate(
            localTransform_,
            glm::radians(angle),
            axis
        );

}



void SceneNode::draw(
    unsigned int shaderProgram,
    const glm::mat4& parentTransform
)
{

    glm::mat4 globalTransform =
        parentTransform *
        localTransform_;



    if(mesh_)
    {
        mesh_->Draw(
            shaderProgram,
            globalTransform
        );
    }



    for(auto child: children_)
    {
        child->draw(
            shaderProgram,
            globalTransform
        );
    }

}
