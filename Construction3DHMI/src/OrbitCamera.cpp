#include "OrbitCamera.h"

#include <algorithm>
#include <cmath>

#include <glm/gtc/matrix_transform.hpp>


OrbitCamera::OrbitCamera(
    UpAxis upAxis
)
    : upAxis_(upAxis)
{
}


void OrbitCamera::setUpAxis(
    UpAxis upAxis
)
{
    upAxis_ = upAxis;
}


UpAxis OrbitCamera::getUpAxis() const
{
    return upAxis_;
}


glm::vec3 OrbitCamera::getWorldUp() const
{
    if (upAxis_ == UpAxis::Z)
    {
        return glm::vec3(
            0.0f,
            0.0f,
            1.0f
        );
    }

    return glm::vec3(
        0.0f,
        1.0f,
        0.0f
    );
}


void OrbitCamera::fit(
    const glm::vec3& center,
    float radius
)
{
    target_ = center;

    modelRadius_ =
        std::max(
            radius,
            0.001f
        );

    distance_ =
        modelRadius_ * 2.5f;

    yaw_ = 45.0f;
    pitch_ = 25.0f;
}


void OrbitCamera::orbit(
    float deltaX,
    float deltaY
)
{
    constexpr float sensitivity = 0.20f;

    yaw_ += deltaX * sensitivity;
    pitch_ += deltaY * sensitivity;

    pitch_ =
        std::clamp(
            pitch_,
            -89.0f,
            89.0f
        );
}


void OrbitCamera::pan(
    float deltaX,
    float deltaY
)
{
    const float panScale =
        distance_ * 0.0015f;

    const glm::vec3 right =
        getRight();

    const glm::vec3 up =
        getCameraUp();

    target_ -=
        right *
        deltaX *
        panScale;

    target_ +=
        up *
        deltaY *
        panScale;
}


void OrbitCamera::zoom(
    float scrollOffset
)
{
    const float zoomFactor =
        std::pow(
            0.90f,
            scrollOffset
        );

    distance_ *= zoomFactor;

    const float minDistance =
        modelRadius_ * 0.05f;

    const float maxDistance =
        modelRadius_ * 100.0f;

    distance_ =
        std::clamp(
            distance_,
            minDistance,
            maxDistance
        );
}


glm::vec3 OrbitCamera::getPosition() const
{
    const float yawRad =
        glm::radians(yaw_);

    const float pitchRad =
        glm::radians(pitch_);

    glm::vec3 offset(0.0f);

    if (upAxis_ == UpAxis::Z)
    {
        // Z-Up:
        // XY 是水平面，Z 是高度
        offset.x =
            distance_ *
            std::cos(pitchRad) *
            std::cos(yawRad);

        offset.y =
            distance_ *
            std::cos(pitchRad) *
            std::sin(yawRad);

        offset.z =
            distance_ *
            std::sin(pitchRad);
    }
    else
    {
        // Y-Up:
        // XZ 是水平面，Y 是高度
        offset.x =
            distance_ *
            std::cos(pitchRad) *
            std::cos(yawRad);

        offset.y =
            distance_ *
            std::sin(pitchRad);

        offset.z =
            distance_ *
            std::cos(pitchRad) *
            std::sin(yawRad);
    }

    return target_ + offset;
}


glm::vec3 OrbitCamera::getForward() const
{
    return glm::normalize(
        target_ -
        getPosition()
    );
}


glm::vec3 OrbitCamera::getRight() const
{
    return glm::normalize(
        glm::cross(
            getForward(),
            getWorldUp()
        )
    );
}


glm::vec3 OrbitCamera::getCameraUp() const
{
    return glm::normalize(
        glm::cross(
            getRight(),
            getForward()
        )
    );
}


glm::mat4 OrbitCamera::getViewMatrix() const
{
    return glm::lookAt(
        getPosition(),
        target_,
        getWorldUp()
    );
}


const glm::vec3&
OrbitCamera::getTarget() const
{
    return target_;
}
