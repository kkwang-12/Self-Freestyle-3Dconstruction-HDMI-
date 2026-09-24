#include "OpenGLWidget.h"

#include "Model.h"
#include "GridRenderer.h"
#include "AxisRenderer.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QOpenGLContext>
#include <QWheelEvent>
#include <QDebug>

#include <algorithm>
#include <stdexcept>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// ============================================================
// Qt -> GLAD OpenGL function loader
// ============================================================

static GLADapiproc qtGetProcAddress(const char* name)
{
    QOpenGLContext* context =
        QOpenGLContext::currentContext();

    if (!context)
    {
        return nullptr;
    }

    return reinterpret_cast<GLADapiproc>(
        context->getProcAddress(name)
    );
}


// ============================================================
// Model Shader
// ============================================================

static const char* MODEL_VERTEX_SHADER = R"(

#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 worldPosition =
        model * vec4(aPos, 1.0);

    FragPos =
        worldPosition.xyz;

    Normal =
        mat3(
            transpose(
                inverse(model)
            )
        ) * aNormal;

    gl_Position =
        projection *
        view *
        worldPosition;
}

)";


static const char* MODEL_FRAGMENT_SHADER = R"(

#version 330 core

in vec3 FragPos;
in vec3 Normal;

out vec4 FragColor;

uniform vec3 cameraPos;

uniform vec4 materialBaseColor;
uniform float materialMetallic;
uniform float materialRoughness;

void main()
{
    vec3 N =
        normalize(Normal);

    vec3 lightDirection =
        normalize(
            vec3(
                -0.6,
                -0.8,
                -1.0
            )
        );

    vec3 L =
        normalize(
            -lightDirection
        );

    float diffuse =
        max(
            dot(N, L),
            0.0
        );

    vec3 V =
        normalize(
            cameraPos -
            FragPos
        );

    vec3 H =
        normalize(
            V + L
        );

    float roughness =
        clamp(
            materialRoughness,
            0.04,
            1.0
        );

    float shininess =
        mix(
            128.0,
            8.0,
            roughness
        );

    float spec =
        pow(
            max(
                dot(N, H),
                0.0
            ),
            shininess
        );

    vec3 baseColor =
        materialBaseColor.rgb;

    vec3 ambient =
        baseColor * 0.38;

    vec3 diffuseColor =
        baseColor *
        diffuse *
        0.62;

    vec3 specularColor =
        mix(
            vec3(0.18),
            baseColor,
            clamp(
                materialMetallic,
                0.0,
                1.0
            )
        );

    vec3 specular =
        specularColor *
        spec *
        0.30;

    vec3 result =
        ambient +
        diffuseColor +
        specular;

    FragColor =
        vec4(
            result,
            materialBaseColor.a
        );
}

)";


// ============================================================
// Grid Shader
// ============================================================

static const char* GRID_VERTEX_SHADER = R"(

#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position =
        projection *
        view *
        vec4(
            aPos,
            1.0
        );
}

)";


static const char* GRID_FRAGMENT_SHADER = R"(

#version 330 core

out vec4 FragColor;

uniform vec3 lineColor;

void main()
{
    FragColor =
        vec4(
            lineColor,
            1.0
        );
}

)";


// ============================================================
// Axis Shader
// ============================================================

static const char* AXIS_VERTEX_SHADER = R"(

#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 Color;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    Color = aColor;

    gl_Position =
        projection *
        view *
        vec4(
            aPos,
            1.0
        );
}

)";


static const char* AXIS_FRAGMENT_SHADER = R"(

#version 330 core

in vec3 Color;

out vec4 FragColor;

void main()
{
    FragColor =
        vec4(
            Color,
            1.0
        );
}

)";


// ============================================================
// Constructor
// ============================================================

OpenGLWidget::OpenGLWidget(QWidget* parent)
    : QOpenGLWidget(parent)
{
    setFocusPolicy(
        Qt::StrongFocus
    );
}


// ============================================================
// Destructor
// ============================================================

OpenGLWidget::~OpenGLWidget()
{
    if (context())
    {
        makeCurrent();

        destroyGLResources();

        doneCurrent();
    }
}


// ============================================================
// OpenGL Initialize
// ============================================================

void OpenGLWidget::initializeGL()
{
    // --------------------------------------------------------
    // Load OpenGL functions through Qt context
    // --------------------------------------------------------

    const int version =
        gladLoadGL(
            qtGetProcAddress
        );

    if (version == 0)
    {
        qFatal(
            "Failed to initialize GLAD."
        );
    }


    qDebug()
        << "OpenGL:"
        << reinterpret_cast<const char*>(
               glGetString(
                   GL_VERSION
               )
           );


    qDebug()
        << "Renderer:"
        << reinterpret_cast<const char*>(
               glGetString(
                   GL_RENDERER
               )
           );


    // --------------------------------------------------------
    // OpenGL state
    // --------------------------------------------------------

    glEnable(
        GL_DEPTH_TEST
    );

    glDepthFunc(
        GL_LESS
    );


    // --------------------------------------------------------
    // Create shaders
    // --------------------------------------------------------

    modelShader_ =
        createShaderProgram(
            MODEL_VERTEX_SHADER,
            MODEL_FRAGMENT_SHADER
        );


    gridShader_ =
        createShaderProgram(
            GRID_VERTEX_SHADER,
            GRID_FRAGMENT_SHADER
        );


    axisShader_ =
        createShaderProgram(
            AXIS_VERTEX_SHADER,
            AXIS_FRAGMENT_SHADER
        );


    // --------------------------------------------------------
    // Locate CET200
    // --------------------------------------------------------

    QDir appDir(
        QCoreApplication::
            applicationDirPath()
    );


    QString modelPath =
        appDir.absoluteFilePath(
            "../assets/models/cet200/cet200.glb"
        );


    // 如果 exe 位于 build/
    // 项目 assets 位于根目录
    if (!QFileInfo::exists(modelPath))
    {
        modelPath =
            QDir::current()
                .absoluteFilePath(
                    "assets/models/cet200/cet200.glb"
                );
    }


    qDebug()
        << "Loading CET200:"
        << modelPath;


    if (!QFileInfo::exists(modelPath))
    {
        qCritical()
            << "CET200 model not found:"
            << modelPath;

        return;
    }


    // --------------------------------------------------------
    // Load Model
    // --------------------------------------------------------

    model_ =
        std::make_unique<Model>(
            modelPath.toStdString()
        );


    // --------------------------------------------------------
    // Model bounds
    // --------------------------------------------------------

    modelCenter_ =
        model_->getCenter();


    modelRadius_ =
        model_->getRadius();


    if (modelRadius_ <= 0.0f)
    {
        modelRadius_ = 1.0f;
    }


    // --------------------------------------------------------
    // Camera Fit
    // --------------------------------------------------------

    camera_.fit(
        modelCenter_,
        modelRadius_
    );


    // --------------------------------------------------------
    // Grid
    // --------------------------------------------------------

    const float gridSize =
        std::max(
            modelRadius_ * 3.0f,
            10.0f
        );


    grid_ =
        std::make_unique<GridRenderer>(
            VIEWER_UP_AXIS,
            gridSize,
            40
        );


    // --------------------------------------------------------
    // Axis
    // --------------------------------------------------------

    const float axisSize =
        std::max(
            modelRadius_ * 0.5f,
            1.0f
        );


    axis_ =
        std::make_unique<AxisRenderer>(
            axisSize
        );


    qDebug()
        << "CET200 loaded successfully.";
}


// ============================================================
// Resize
// ============================================================

void OpenGLWidget::resizeGL(
    int w,
    int h
)
{
    glViewport(
        0,
        0,
        std::max(w, 1),
        std::max(h, 1)
    );
}


// ============================================================
// Render
// ============================================================

void OpenGLWidget::paintGL()
{
    // --------------------------------------------------------
    // Background
    // --------------------------------------------------------

    glClearColor(
        0.035f,
        0.050f,
        0.065f,
        1.0f
    );


    glClear(
        GL_COLOR_BUFFER_BIT |
        GL_DEPTH_BUFFER_BIT
    );


    if (!model_)
    {
        return;
    }


    // --------------------------------------------------------
    // Camera
    // --------------------------------------------------------

    const glm::mat4 view =
        camera_.getViewMatrix();


    const float aspect =
        static_cast<float>(
            std::max(
                width(),
                1
            )
        )
        /
        static_cast<float>(
            std::max(
                height(),
                1
            )
        );


    const float nearPlane =
        std::max(
            modelRadius_ *
                0.001f,
            0.001f
        );


    const float farPlane =
        std::max(
            modelRadius_ *
                1000.0f,
            100.0f
        );


    const glm::mat4 projection =
        glm::perspective(
            glm::radians(
                45.0f
            ),
            aspect,
            nearPlane,
            farPlane
        );


    // ========================================================
    // GRID
    // ========================================================

    if (grid_)
    {
        glUseProgram(
            gridShader_
        );


        glUniformMatrix4fv(
            glGetUniformLocation(
                gridShader_,
                "view"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(
                view
            )
        );


        glUniformMatrix4fv(
            glGetUniformLocation(
                gridShader_,
                "projection"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(
                projection
            )
        );


        grid_->Draw(
            gridShader_
        );
    }


    // ========================================================
    // AXIS
    // ========================================================

    if (axis_)
    {
        glUseProgram(
            axisShader_
        );


        glUniformMatrix4fv(
            glGetUniformLocation(
                axisShader_,
                "view"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(
                view
            )
        );


        glUniformMatrix4fv(
            glGetUniformLocation(
                axisShader_,
                "projection"
            ),
            1,
            GL_FALSE,
            glm::value_ptr(
                projection
            )
        );


        axis_->Draw(
            axisShader_
        );
    }


    // ========================================================
    // CET200 MODEL
    // ========================================================

    glUseProgram(
        modelShader_
    );


    const glm::mat4 modelMatrix(
        1.0f
    );


    glUniformMatrix4fv(
        glGetUniformLocation(
            modelShader_,
            "model"
        ),
        1,
        GL_FALSE,
        glm::value_ptr(
            modelMatrix
        )
    );


    glUniformMatrix4fv(
        glGetUniformLocation(
            modelShader_,
            "view"
        ),
        1,
        GL_FALSE,
        glm::value_ptr(
            view
        )
    );


    glUniformMatrix4fv(
        glGetUniformLocation(
            modelShader_,
            "projection"
        ),
        1,
        GL_FALSE,
        glm::value_ptr(
            projection
        )
    );


    const glm::vec3 cameraPosition =
        camera_.getPosition();


    glUniform3fv(
        glGetUniformLocation(
            modelShader_,
            "cameraPos"
        ),
        1,
        glm::value_ptr(
            cameraPosition
        )
    );


    model_->Draw(
        modelShader_
    );


    glUseProgram(0);
}


// ============================================================
// Mouse Press
// ============================================================

void OpenGLWidget::mousePressEvent(
    QMouseEvent* event
)
{
    lastMousePos_ =
        event
            ->position()
            .toPoint();


    if (
        event->button() ==
        Qt::LeftButton
    )
    {
        leftMouseDown_ = true;
    }


    if (
        event->button() ==
        Qt::RightButton
    )
    {
        rightMouseDown_ = true;
    }


    setFocus();


    event->accept();
}


// ============================================================
// Mouse Release
// ============================================================

void OpenGLWidget::mouseReleaseEvent(
    QMouseEvent* event
)
{
    if (
        event->button() ==
        Qt::LeftButton
    )
    {
        leftMouseDown_ = false;
    }


    if (
        event->button() ==
        Qt::RightButton
    )
    {
        rightMouseDown_ = false;
    }


    event->accept();
}


// ============================================================
// Mouse Move
// ============================================================

void OpenGLWidget::mouseMoveEvent(
    QMouseEvent* event
)
{
    const QPoint current =
        event
            ->position()
            .toPoint();


    const QPoint delta =
        current -
        lastMousePos_;


    lastMousePos_ =
        current;


    // --------------------------------------------------------
    // Orbit
    // --------------------------------------------------------

    if (leftMouseDown_)
    {
        camera_.orbit(
            static_cast<float>(
                delta.x()
            ),
            static_cast<float>(
                delta.y()
            )
        );


        update();
    }


    // --------------------------------------------------------
    // Pan
    // --------------------------------------------------------

    if (rightMouseDown_)
    {
        camera_.pan(
            static_cast<float>(
                delta.x()
            ),
            static_cast<float>(
                delta.y()
            )
        );


        update();
    }


    event->accept();
}


// ============================================================
// Mouse Wheel
// ============================================================

void OpenGLWidget::wheelEvent(
    QWheelEvent* event
)
{
    const float steps =
        static_cast<float>(
            event
                ->angleDelta()
                .y()
        )
        /
        120.0f;


    camera_.zoom(
        steps
    );


    update();


    event->accept();
}


// ============================================================
// Keyboard
// ============================================================

void OpenGLWidget::keyPressEvent(
    QKeyEvent* event
)
{
    // F = Fit model
    if (
        event->key() ==
        Qt::Key_F
    )
    {
        if (model_)
        {
            camera_.fit(
                modelCenter_,
                modelRadius_
            );


            update();
        }


        event->accept();

        return;
    }


    QOpenGLWidget::
        keyPressEvent(
            event
        );
}


// ============================================================
// Compile Shader
// ============================================================

GLuint OpenGLWidget::compileShader(
    GLenum type,
    const char* source
)
{
    const GLuint shader =
        glCreateShader(
            type
        );


    glShaderSource(
        shader,
        1,
        &source,
        nullptr
    );


    glCompileShader(
        shader
    );


    GLint success = GL_FALSE;


    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );


    if (success != GL_TRUE)
    {
        GLchar infoLog[4096]{};


        glGetShaderInfoLog(
            shader,
            sizeof(infoLog),
            nullptr,
            infoLog
        );


        qCritical()
            << "Shader compile error:"
            << infoLog;


        glDeleteShader(
            shader
        );


        return 0;
    }


    return shader;
}


// ============================================================
// Create Shader Program
// ============================================================

GLuint OpenGLWidget::createShaderProgram(
    const char* vertexSource,
    const char* fragmentSource
)
{
    const GLuint vertexShader =
        compileShader(
            GL_VERTEX_SHADER,
            vertexSource
        );


    const GLuint fragmentShader =
        compileShader(
            GL_FRAGMENT_SHADER,
            fragmentSource
        );


    if (
        vertexShader == 0 ||
        fragmentShader == 0
    )
    {
        if (vertexShader)
        {
            glDeleteShader(
                vertexShader
            );
        }


        if (fragmentShader)
        {
            glDeleteShader(
                fragmentShader
            );
        }


        return 0;
    }


    const GLuint program =
        glCreateProgram();


    glAttachShader(
        program,
        vertexShader
    );


    glAttachShader(
        program,
        fragmentShader
    );


    glLinkProgram(
        program
    );


    GLint success =
        GL_FALSE;


    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &success
    );


    if (success != GL_TRUE)
    {
        GLchar infoLog[4096]{};


        glGetProgramInfoLog(
            program,
            sizeof(infoLog),
            nullptr,
            infoLog
        );


        qCritical()
            << "Shader link error:"
            << infoLog;


        glDeleteProgram(
            program
        );


        glDeleteShader(
            vertexShader
        );


        glDeleteShader(
            fragmentShader
        );


        return 0;
    }


    glDeleteShader(
        vertexShader
    );


    glDeleteShader(
        fragmentShader
    );


    return program;
}


// ============================================================
// Cleanup
// ============================================================

void OpenGLWidget::destroyGLResources()
{
    // 这些对象内部可能持有 VAO/VBO/EBO，
    // 必须在 Qt OpenGL Context 仍有效时销毁。

    axis_.reset();

    grid_.reset();

    model_.reset();


    if (modelShader_ != 0)
    {
        glDeleteProgram(
            modelShader_
        );

        modelShader_ = 0;
    }


    if (gridShader_ != 0)
    {
        glDeleteProgram(
            gridShader_
        );

        gridShader_ = 0;
    }


    if (axisShader_ != 0)
    {
        glDeleteProgram(
            axisShader_
        );

        axisShader_ = 0;
    }
}
