cmake_minimum_required(VERSION 3.20)

project(Construction3DHMI LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS
    Widgets
    OpenGL
    OpenGLWidgets
)

find_package(assimp REQUIRED)

add_executable(Construction3DHMI

    src/main.cpp

    src/MainWindow.cpp
    src/MainWindow.h

    src/OpenGLWidget.cpp
    src/OpenGLWidget.h

    src/Mesh.cpp
    src/Mesh.h

    src/Model.cpp
    src/Model.h

    src/Material.cpp
    src/Material.h

    src/OrbitCamera.cpp
    src/OrbitCamera.h

    src/GridRenderer.cpp
    src/GridRenderer.h

    src/AxisRenderer.cpp
    src/AxisRenderer.h

    src/VehicleState.cpp
    src/VehicleState.h

    src/VehicleSimulator.cpp
    src/VehicleSimulator.h

    src/CircularGauge.cpp
    src/CircularGauge.h
    external/glad/src/gl.c
)

target_include_directories(Construction3DHMI PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
)

target_link_libraries(Construction3DHMI PRIVATE
    Qt6::Widgets
    Qt6::OpenGL
    Qt6::OpenGLWidgets
    assimp::assimp
)
