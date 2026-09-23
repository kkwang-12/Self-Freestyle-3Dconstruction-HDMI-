#pragma once

#include <glm/glm.hpp>

enum class UpAxis
{
    Y,
    Z
};

class OrbitCamera
{
public:
    explicit OrbitCamera(UpAxis upAxis = UpAxis::Y);

    void setUpAxis(UpAxis upAxis);
    UpAxis getUpAxis() const;

    void fit(
        const glm::vec3& center,
        float radius
    );

    void orbit(
        float deltaX,
        float deltaY
    );

    void pan(
        float deltaX,
        float deltaY
    );

    void zoom(
        float scrollOffset
    );

    glm::mat4 getViewMatrix() const;

    glm::vec3 getPosition() const;
    glm::vec3 getWorldUp() const;

    const glm::vec3& getTarget() const;

private:
    UpAxis upAxis_;

    glm::vec3 target_{0.0f};

    float distance_ = 5.0f;

    float yaw_ = 45.0f;
    float pitch_ = 25.0f;

    float modelRadius_ = 1.0f;

    glm::vec3 getForward() const;
    glm::vec3 getRight() const;
    glm::vec3 getCameraUp() const;
};
