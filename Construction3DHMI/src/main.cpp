#include <algorithm>
#include <iostream>
#include <string>

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Model.h"
#include "OrbitCamera.h"
#include "GridRenderer.h"
#include "AxisRenderer.h"


// ============================================================
// Window
// ============================================================

int framebufferWidth = 1280;
int framebufferHeight = 720;


// ============================================================
// Viewer Coordinate System
// ============================================================

constexpr UpAxis VIEWER_UP_AXIS =
    UpAxis::Y;


// ============================================================
// Camera
// ============================================================

OrbitCamera camera(
    VIEWER_UP_AXIS
);


// ============================================================
// Mouse
// ============================================================

bool leftMouseDown = false;
bool rightMouseDown = false;

double lastMouseX = 0.0;
double lastMouseY = 0.0;


// ============================================================
// Model info
// ============================================================

glm::vec3 modelCenter(0.0f);
float modelRadius = 1.0f;


// ============================================================
// Model Vertex Shader
// ============================================================

const char* modelVertexShaderSource = R"(

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
        model *
        vec4(aPos, 1.0);

    FragPos =
        worldPosition.xyz;

    Normal =
        mat3(
            transpose(
                inverse(model)
            )
        )
        *
        aNormal;

    gl_Position =
        projection *
        view *
        worldPosition;
}

)";


// ============================================================
// Model Fragment Shader
// ============================================================

const char* modelFragmentShaderSource = R"(

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

    // 稍微增强环境光，避免模型暗面完全黑掉
    vec3 ambient =
        baseColor *
        0.38;

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

const char* gridVertexShaderSource = R"(

#version 330 core

layout(location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position =
        projection *
        view *
        vec4(aPos, 1.0);
}

)";


const char* gridFragmentShaderSource = R"(

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

const char* axisVertexShaderSource = R"(

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
        vec4(aPos, 1.0);
}

)";


const char* axisFragmentShaderSource = R"(

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
// Shader helper
// ============================================================

unsigned int compileShader(
    unsigned int type,
    const char* source
)
{
    unsigned int shader =
        glCreateShader(type);

    glShaderSource(
        shader,
        1,
        &source,
        nullptr
    );

    glCompileShader(shader);

    int success = 0;

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );

    if (!success)
    {
        char infoLog[2048];

        glGetShaderInfoLog(
            shader,
            2048,
            nullptr,
            infoLog
        );

        std::cerr
            << "Shader compile error:\n"
            << infoLog
            << std::endl;
    }

    return shader;
}


unsigned int createShaderProgram(
    const char* vertexSource,
    const char* fragmentSource
)
{
    unsigned int vertexShader =
        compileShader(
            GL_VERTEX_SHADER,
            vertexSource
        );

    unsigned int fragmentShader =
        compileShader(
            GL_FRAGMENT_SHADER,
            fragmentSource
        );

    unsigned int program =
        glCreateProgram();

    glAttachShader(
        program,
        vertexShader
    );

    glAttachShader(
        program,
        fragmentShader
    );

    glLinkProgram(program);

    int success = 0;

    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &success
    );

    if (!success)
    {
        char infoLog[2048];

        glGetProgramInfoLog(
            program,
            2048,
            nullptr,
            infoLog
        );

        std::cerr
            << "Shader link error:\n"
            << infoLog
            << std::endl;
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
// Callbacks
// ============================================================

void framebufferCallback(
    GLFWwindow* window,
    int width,
    int height
)
{
    (void)window;

    framebufferWidth =
        std::max(
            width,
            1
        );

    framebufferHeight =
        std::max(
            height,
            1
        );

    glViewport(
        0,
        0,
        framebufferWidth,
        framebufferHeight
    );
}


void mouseButtonCallback(
    GLFWwindow* window,
    int button,
    int action,
    int mods
)
{
    (void)mods;

    if (
        button ==
        GLFW_MOUSE_BUTTON_LEFT
    )
    {
        leftMouseDown =
            action ==
            GLFW_PRESS;
    }

    if (
        button ==
        GLFW_MOUSE_BUTTON_RIGHT
    )
    {
        rightMouseDown =
            action ==
            GLFW_PRESS;
    }

    if (
        action ==
        GLFW_PRESS
    )
    {
        glfwGetCursorPos(
            window,
            &lastMouseX,
            &lastMouseY
        );
    }
}


void cursorCallback(
    GLFWwindow* window,
    double xpos,
    double ypos
)
{
    (void)window;

    const float dx =
        static_cast<float>(
            xpos -
            lastMouseX
        );

    const float dy =
        static_cast<float>(
            ypos-lastMouseY
        );

    lastMouseX = xpos;
    lastMouseY = ypos;

    if (leftMouseDown)
    {
        camera.orbit(
            dx,
            dy
        );
    }

    if (rightMouseDown)
    {
        camera.pan(
            dx,
            dy
        );
    }
}


void scrollCallback(
    GLFWwindow* window,
    double xoffset,
    double yoffset
)
{
    (void)window;
    (void)xoffset;

    camera.zoom(
        static_cast<float>(
            yoffset
        )
    );
}


// ============================================================
// Input
// ============================================================

void processInput(
    GLFWwindow* window
)
{
    if (
        glfwGetKey(
            window,
            GLFW_KEY_ESCAPE
        )
        ==
        GLFW_PRESS
    )
    {
        glfwSetWindowShouldClose(
            window,
            true
        );
    }

    static bool lastF = false;

    const bool currentF =
        glfwGetKey(
            window,
            GLFW_KEY_F
        )
        ==
        GLFW_PRESS;

    if (
        currentF &&
        !lastF
    )
    {
        camera.fit(
            modelCenter,
            modelRadius
        );
    }

    lastF = currentF;
}


// ============================================================
// Main
// ============================================================

int main(
    int argc,
    char* argv[]
)
{
    if (!glfwInit())
    {
        std::cerr
            << "Failed to initialize GLFW."
            << std::endl;

        return -1;
    }

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3
    );

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3
    );

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE
    );

    GLFWwindow* window =
        glfwCreateWindow(
            framebufferWidth,
            framebufferHeight,
            "Construction3DHMI - 3D Viewer",
            nullptr,
            nullptr
        );

    if (!window)
    {
        std::cerr
            << "Failed to create GLFW window."
            << std::endl;

        glfwTerminate();

        return -1;
    }

    glfwMakeContextCurrent(
        window
    );

    glfwSwapInterval(1);

    if (!gladLoadGL(
            glfwGetProcAddress
        ))
    {
        std::cerr
            << "Failed to initialize GLAD."
            << std::endl;

        glfwDestroyWindow(
            window
        );

        glfwTerminate();

        return -1;
    }

    glfwSetFramebufferSizeCallback(
        window,
        framebufferCallback
    );

    glfwSetMouseButtonCallback(
        window,
        mouseButtonCallback
    );

    glfwSetCursorPosCallback(
        window,
        cursorCallback
    );

    glfwSetScrollCallback(
        window,
        scrollCallback
    );

    glEnable(
        GL_DEPTH_TEST
    );


    // ========================================================
    // Shader Programs
    // ========================================================

    const unsigned int modelShader =
        createShaderProgram(
            modelVertexShaderSource,
            modelFragmentShaderSource
        );

    const unsigned int gridShader =
        createShaderProgram(
            gridVertexShaderSource,
            gridFragmentShaderSource
        );

    const unsigned int axisShader =
        createShaderProgram(
            axisVertexShaderSource,
            axisFragmentShaderSource
        );


    // ========================================================
    // Model path
    // ========================================================

    std::string modelPath =
        "../assets/models/cet200/cet200.glb";

    if (argc >= 2)
    {
        modelPath =
            argv[1];
    }

    std::cout
        << "Loading model: "
        << modelPath
        << std::endl;


    try
    {
        Model model(
            modelPath
        );

        modelCenter =
            model.getCenter();

        modelRadius =
            model.getRadius();

        camera.fit(
            modelCenter,
            modelRadius
        );


        // ====================================================
        // Viewer Helpers
        // ====================================================

        const float gridSize =
            std::max(
                modelRadius * 3.0f,
                10.0f
            );

        GridRenderer grid(
            VIEWER_UP_AXIS,
            gridSize,
            40
        );

        AxisRenderer axis(
            std::max(
                modelRadius * 0.5f,
                1.0f
            )
        );


        // ====================================================
        // Render Loop
        // ====================================================

        while (
            !glfwWindowShouldClose(
                window
            )
        )
        {
            processInput(
                window
            );

            glClearColor(
                0.075f,
                0.080f,
                0.095f,
                1.0f
            );

            glClear(
                GL_COLOR_BUFFER_BIT |
                GL_DEPTH_BUFFER_BIT
            );


            const glm::mat4 view =
                camera.getViewMatrix();

            const float aspect =
                static_cast<float>(
                    framebufferWidth
                )
                /
                static_cast<float>(
                    framebufferHeight
                );

            const float nearPlane =
                std::max(
                    modelRadius * 0.001f,
                    0.001f
                );

            const float farPlane =
                std::max(
                    modelRadius * 1000.0f,
                    100.0f
                );

            const glm::mat4 projection =
                glm::perspective(
                    glm::radians(45.0f),
                    aspect,
                    nearPlane,
                    farPlane
                );


            // =================================================
            // Grid
            // =================================================

            glUseProgram(
                gridShader
            );

            glUniformMatrix4fv(
                glGetUniformLocation(
                    gridShader,
                    "view"
                ),
                1,
                GL_FALSE,
                glm::value_ptr(view)
            );

            glUniformMatrix4fv(
                glGetUniformLocation(
                    gridShader,
                    "projection"
                ),
                1,
                GL_FALSE,
                glm::value_ptr(
                    projection
                )
            );

            grid.Draw(
                gridShader
            );


            // =================================================
            // Axis
            // =================================================

            glUseProgram(
                axisShader
            );

            glUniformMatrix4fv(
                glGetUniformLocation(
                    axisShader,
                    "view"
                ),
                1,
                GL_FALSE,
                glm::value_ptr(view)
            );

            glUniformMatrix4fv(
                glGetUniformLocation(
                    axisShader,
                    "projection"
                ),
                1,
                GL_FALSE,
                glm::value_ptr(
                    projection
                )
            );

            axis.Draw(
                axisShader
            );


            // =================================================
            // Model
            // =================================================

            glUseProgram(
                modelShader
            );

            const glm::mat4 modelMatrix(
                1.0f
            );

            glUniformMatrix4fv(
                glGetUniformLocation(
                    modelShader,
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
                    modelShader,
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
                    modelShader,
                    "projection"
                ),
                1,
                GL_FALSE,
                glm::value_ptr(
                    projection
                )
            );

            const glm::vec3 cameraPosition =
                camera.getPosition();

            glUniform3fv(
                glGetUniformLocation(
                    modelShader,
                    "cameraPos"
                ),
                1,
                glm::value_ptr(
                    cameraPosition
                )
            );

            model.Draw(
                modelShader
            );


            // =================================================
            // Present
            // =================================================

            glfwSwapBuffers(
                window
            );

            glfwPollEvents();
        }
    }
    catch (
        const std::exception& e
    )
    {
        std::cerr
            << "Error: "
            << e.what()
            << std::endl;

        glDeleteProgram(
            modelShader
        );

        glDeleteProgram(
            gridShader
        );

        glDeleteProgram(
            axisShader
        );

        glfwDestroyWindow(
            window
        );

        glfwTerminate();

        return -1;
    }


    glDeleteProgram(
        modelShader
    );

    glDeleteProgram(
        gridShader
    );

    glDeleteProgram(
        axisShader
    );

    glfwDestroyWindow(
        window
    );

    glfwTerminate();

    return 0;
}
