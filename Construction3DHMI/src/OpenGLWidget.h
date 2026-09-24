#pragma once

// GLAD 必须在 Qt OpenGL 相关头文件之前
#include <glad/gl.h>

#include <QOpenGLWidget>
#include <QPoint>

#include <memory>

#include <glm/glm.hpp>

#include "OrbitCamera.h"

class Model;
class GridRenderer;
class AxisRenderer;

class QMouseEvent;
class QWheelEvent;
class QKeyEvent;

class OpenGLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit OpenGLWidget(QWidget* parent = nullptr);
    ~OpenGLWidget() override;

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    static constexpr UpAxis VIEWER_UP_AXIS = UpAxis::Y;

    OrbitCamera camera_{VIEWER_UP_AXIS};

    std::unique_ptr<Model> model_;
    std::unique_ptr<GridRenderer> grid_;
    std::unique_ptr<AxisRenderer> axis_;

    GLuint modelShader_ = 0;
    GLuint gridShader_ = 0;
    GLuint axisShader_ = 0;

    glm::vec3 modelCenter_{0.0f};
    float modelRadius_ = 1.0f;

    QPoint lastMousePos_;

    bool leftMouseDown_ = false;
    bool rightMouseDown_ = false;

private:
    GLuint compileShader(
        GLenum type,
        const char* source
    );

    GLuint createShaderProgram(
        const char* vertexSource,
        const char* fragmentSource
    );

    void destroyGLResources();
};
