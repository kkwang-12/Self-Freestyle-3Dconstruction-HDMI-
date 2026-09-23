cmake_minimum_required(VERSION 3.20)

project(Construction3DHMI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
# GLFW glfw库文件有自己独立的cmake，交给他自己去找就行
add_subdirectory(external/glfw)
add_subdirectory(external/glm)
add_subdirectory(external/assimp)


# 我们自己的程序 + GLAD
add_executable(Construction3DHMI
    src/main.cpp
    external/glad/src/gl.c
    src/Material.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/OrbitCamera.cpp
    src/GridRenderer.cpp
    src/AxisRenderer.cpp
    external/glad/src/gl.c
)

# GLAD 头文件位置
target_include_directories(Construction3DHMI
    PRIVATE
    external/glad/include
    external/glfw/include
)

# 链接 GLFW
target_link_libraries(Construction3DHMI
    PRIVATE glfw
    glm::glm
    assimp
)
