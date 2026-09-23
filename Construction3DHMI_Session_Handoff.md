# Construction3DHMI — Session Handoff

> 用途：快速恢复项目代码开发上下文。本文仅保留已经明确形成的代码开发主线、当前稳定状态、工程结构和后续路线；不记录回退过程、问答、试错和讨论。

## 1. 项目目标

项目名称：`Construction3DHMI`

当前个人开发主线是构建一个面向工程机械的实时三维 HMI / 数字孪生显示底座。

最终目标不是做通用 Blender/CAD，而是逐步形成：

```text
本地工程机械模型
    ↓
3D Viewer
    ↓
机械部件层级 / Scene Graph
    ↓
Boom / Arm / Bucket / Slew 关节运动
    ↓
Vehicle State
    ↓
CAN / ROS / 传感器实时数据
    ↓
工程机械实时 3D HMI
```

当前测试模型采用 CET200 挖掘机。

---

## 2. 开发环境

平台：

```text
Windows 11
VS Code
MSYS2 / GCC 15.2.0
CMake 4.2.3
Git
C++17
OpenGL 3.3 Core
```

第三方依赖：

```text
external/
├── glfw/
├── glad/
├── glm/
└── assimp/
```

其中：

- GLFW：窗口、鼠标、键盘输入。
- GLAD 2：OpenGL 函数加载。
- GLM：矩阵、向量、Camera Transform。
- Assimp：GLB/GLTF/OBJ/FBX 等模型导入。

GLAD 当前接口：

```cpp
#include <glad/gl.h>

gladLoadGL(glfwGetProcAddress);
```

---

## 3. 当前工程目录主结构

当前工程大致为：

```text
Construction3DHMI/
│
├── CMakeLists.txt
│
├── assets/
│   └── models/
│       └── cet200/
│           ├── cet200.glb
│           └── parts/
│               ├── arm.glb
│               ├── boom.glb
│               ├── bucket.glb
│               ├── collider_bucket.glb
│               ├── crawler_belt.glb
│               ├── idler.glb
│               ├── roller.glb
│               ├── sprocket.glb
│               ├── track_frame.glb
│               ├── track_shoelink.glb
│               └── upper_structure.glb
│
├── external/
│   ├── glfw/
│   ├── glad/
│   ├── glm/
│   └── assimp/
│
├── src/
│   ├── main.cpp
│   ├── Mesh.h
│   ├── Mesh.cpp
│   ├── Model.h
│   ├── Model.cpp
│   ├── Material.h
│   ├── OrbitCamera.h
│   ├── OrbitCamera.cpp
│   ├── GridRenderer.h
│   ├── GridRenderer.cpp
│   ├── AxisRenderer.h
│   └── AxisRenderer.cpp
│
└── build/
```

注意：SceneNode 相关重构尚未作为当前稳定主线正式落地。恢复开发时，应以“稳定 3D Viewer”版本为起点。

---

## 4. 当前稳定功能

当前已经形成一个最小可用 3D Viewer。

### 4.1 OpenGL 基础链路

已经完成：

```text
GLFW Window
    ↓
GLAD
    ↓
OpenGL Shader
    ↓
VAO / VBO / EBO
    ↓
3D Mesh
```

已经从 Triangle、Cube 阶段进入真实模型加载。

### 4.2 Assimp 模型加载

当前：

```cpp
Model model(
    "assets/models/cet200/cet200.glb"
);
```

模型加载链：

```text
GLB
 ↓
Assimp
 ↓
aiScene
 ↓
aiNode
 ↓
aiMesh
 ↓
Vertex / Index
 ↓
Mesh
 ↓
OpenGL
```

整机 CET200 已经能够正常显示。

### 4.3 Mesh

当前 Vertex 核心结构：

```cpp
struct Vertex
{
    glm::vec3 Position;
    glm::vec3 Normal;
};
```

Mesh 负责：

```text
vertices
indices
VAO
VBO
EBO
Material
Draw()
```

### 4.4 Material

已经建立独立 `Material.h`，核心结构：

```cpp
struct Material
{
    glm::vec4 baseColor;
    float metallic;
    float roughness;
};
```

Model 通过 Assimp 读取 GLB Material：

```text
aiMesh::mMaterialIndex
        ↓
aiMaterial
        ↓
AI_MATKEY_BASE_COLOR
AI_MATKEY_METALLIC_FACTOR
AI_MATKEY_ROUGHNESS_FACTOR
        ↓
Material
        ↓
Mesh
        ↓
Shader Uniform
```

CET200 已经恢复多材质颜色，不再整机统一为写死颜色。

当前已经可以看到：

```text
车身主体颜色
履带深色
驾驶室/玻璃深色区域
液压机构颜色差异
Logo 等材质差异
```

当前不是完整 PBR，只是：

```text
BaseColor
+
简单 Ambient
+
Diffuse
+
Specular
+
Metallic/Roughness 近似
```

Texture 尚未正式实现。

---

## 5. Camera

已经实现 `OrbitCamera`。

当前 Viewer 操作：

```text
左键拖动 → Orbit
右键拖动 → Pan
滚轮     → Zoom
F        → Fit Model
ESC      → Exit
```

Camera 不再采用 FPS Camera。

Viewer 使用围绕目标模型旋转的 Orbit Camera，更适合工程机械 HMI / CAD Viewer。

鼠标 Y 方向已经按偏好调整：

```text
鼠标向上拖
    ↓
视角向下绕

鼠标向下拖
    ↓
视角向上绕
```

Orbit 与 Pan 的 Y 增量应分开处理，避免反转 Orbit 时同时改变 Pan 操作习惯。

---

## 6. Bounding Box / Auto Fit

Model 当前维护：

```cpp
boundsMin
boundsMax
```

并提供：

```cpp
getCenter()
getRadius()
```

Viewer 加载模型后：

```cpp
modelCenter = model.getCenter();
modelRadius = model.getRadius();

camera.fit(
    modelCenter,
    modelRadius
);
```

因此模型加载后可以自动居中并调整 Camera 距离。

---

## 7. Grid / Axis

Viewer 已增加：

```text
Ground Grid
XYZ Axis
```

独立 Renderer：

```text
GridRenderer
AxisRenderer
```

辅助图形与真实模型 Shader 分离：

```text
modelShader
gridShader
axisShader
```

这样辅助图形不会污染模型 Material 渲染。

XYZ 颜色约定：

```text
X = Red
Y = Green
Z = Blue
```

---

## 8. 当前坐标系结论

这里非常重要。

CET200 的机器人/机械定义侧是 Z-Up，但当前 `cet200.glb` 经过现有 Assimp/OpenGL 加载链显示时，Viewer 使用 Y-Up 才能正常“站立”。

因此当前稳定 Viewer：

```cpp
constexpr UpAxis VIEWER_UP_AXIS =
    UpAxis::Y;
```

不要因为机器人侧是 Z-Up 就直接把 Viewer 改成 Z-Up。

必须区分：

```text
GLB Model Space
        ≠
Robot / URDF Mechanical Space
```

后续正确架构应该显式增加：

```text
Model Space
    ↓
Coordinate Conversion
    ↓
Scene / Mechanical Space
    ↓
Renderer
```

不要通过修改 Camera 来“修正”模型坐标。

后续进入机械运动前，需要进一步确认 CET200：

```text
GLB axis
URDF axis
joint origin
joint axis
```

之间的映射。

---

## 9. Shader 当前职责

Vertex Shader：

```text
Position
Normal
Model Matrix
View Matrix
Projection Matrix
```

核心数学关系：

```text
gl_Position
=
Projection
*
View
*
Model
*
Position
```

Fragment Shader：

```text
Material BaseColor
Metallic
Roughness
Normal
Camera Position
Light
```

用于计算当前基础 Viewer 光照。

当前理解约定：

```text
Model.cpp
→ 模型是什么

Shader
→ 模型怎么显示
```

---

## 10. 当前 CMake 主结构

核心结构应保持类似：

```cmake
cmake_minimum_required(VERSION 3.20)

project(Construction3DHMI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/glfw)
add_subdirectory(external/glm)

set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)

add_subdirectory(external/assimp)

add_executable(
    Construction3DHMI

    src/main.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/OrbitCamera.cpp
    src/GridRenderer.cpp
    src/AxisRenderer.cpp

    external/glad/src/gl.c
)

target_include_directories(
    Construction3DHMI
    PRIVATE
    external/glad/include
)

target_link_libraries(
    Construction3DHMI
    PRIVATE
    glfw
    glm::glm
    assimp
)
```

---

## 11. 当前里程碑

目前可以把 Stage 1 认定为完成。

```text
Stage 1 — Basic 3D Viewer
─────────────────────────
✓ OpenGL Window
✓ Shader
✓ Depth Test
✓ Assimp
✓ GLB Loading
✓ Mesh
✓ Position
✓ Normal
✓ Material BaseColor
✓ Metallic
✓ Roughness
✓ Orbit Camera
✓ Pan
✓ Zoom
✓ Fit
✓ Bounding Box
✓ Grid
✓ XYZ Axis
✓ CET200 整机显示
```

当前程序性质已经从：

```text
OpenGL Demo
```

升级为：

```text
Minimal Engineering 3D Viewer
```

---

# 12. 下一阶段目标

下一阶段不要继续堆 Viewer 特效。

主线应转向：

```text
3D Viewer
    ↓
Transform System
    ↓
Scene Graph
    ↓
CET200 Semantic Parts
    ↓
Mechanical Joints
    ↓
Vehicle State
```

---

## 13. 下一步第一任务：Transform System

不要直接一次性重构 Scene Graph。

先给当前 Model 增加一个外部：

```cpp
glm::mat4 modelTransform;
```

目标调用：

```cpp
model.Draw(
    modelShader,
    modelTransform
);
```

Transform 数据链：

```text
main
 ↓
Model Transform
 ↓
Model
 ↓
Mesh
 ↓
Shader uniform "model"
 ↓
GPU
```

先测试整机旋转：

```cpp
glm::mat4 modelTransform =
    glm::rotate(
        glm::mat4(1.0f),
        glm::radians(30.0f),
        glm::vec3(0, 1, 0)
    );
```

如果整机正确旋转，则 Model Matrix 传递链完成。

这一阶段尽量不要同时修改 Assimp Node Transform、Material 和 Bounding Box。

---

## 14. Transform 完成后：Scene Graph

Transform 链稳定后，再新增：

```text
SceneNode.h
SceneNode.cpp
```

建议 SceneNode 负责：

```cpp
class SceneNode
{
    std::string name;

    glm::mat4 localTransform;

    Model* model;

    std::vector<SceneNode*> children;
};
```

核心关系：

```text
globalTransform
=
parentGlobalTransform
*
localTransform
```

渲染：

```text
Root
 ↓
Upper
 ↓
Boom
 ↓
Arm
 ↓
Bucket
```

最终：

```text
Bucket Global Matrix
=
Root
*
Upper
*
Boom
*
Arm
*
Bucket
```

这与 ROS TF Tree、URDF Link/Joint、游戏引擎 Scene Graph 是同一类思想。

---

## 15. CET200 部件层级

当前 CET200 已经有独立零件文件：

```text
upper_structure.glb
boom.glb
arm.glb
bucket.glb
```

初期只关注这四个。

暂时忽略：

```text
collider_bucket.glb
crawler_belt.glb
idler.glb
roller.glb
sprocket.glb
track_frame.glb
track_shoelink.glb
```

第一版机械层级目标：

```text
ExcavatorRoot
      │
      └── UpperStructure
              │
              └── Boom
                    │
                    └── Arm
                          │
                          └── Bucket
```

后续再增加：

```text
Slew
Crawler
Track
```

---

## 16. 机械运动阶段不要猜 Pivot

非常重要：

不能简单：

```cpp
glm::rotate(mesh, angle, Y);
```

然后凭视觉调整。

正确机械运动需要：

```text
Joint Origin
Joint Axis
Parent Link
Child Link
```

因此进入 Boom / Arm / Bucket 动画之前，应读取 CET200 的 URDF/Xacro 或其他机械定义，确定：

```text
Slew joint
Boom joint
Arm joint
Bucket joint
```

各自：

```text
origin xyz
origin rpy
axis xyz
parent
child
```

然后构造：

```text
T_joint
*
R_joint(angle)
*
T_joint^-1
```

或按照 URDF 层级建立正确局部 Transform。

不要手工猜旋转中心。

---

## 17. 目标机械状态接口

机械层稳定后，建立独立状态结构，而不是让 CAN 代码直接操作 OpenGL：

```cpp
struct ExcavatorState
{
    float slewAngle;
    float boomAngle;
    float armAngle;
    float bucketAngle;
};
```

正确数据流：

```text
CAN / ROS / Simulator
        ↓
Parser
        ↓
ExcavatorState
        ↓
Scene / Kinematics
        ↓
SceneNode Transform
        ↓
Renderer
```

这样后面输入源可以自由替换：

```text
Keyboard
Slider
Simulated CAN
Real CAN
ROS joint_states
```

Renderer 不需要知道数据来自哪里。

---

## 18. ROS 经验与本项目映射

用户已有 ROS/Gazebo/SLAM/Navigation/TF 基础，可直接映射：

```text
ROS / Gazebo              Construction3DHMI

URDF                →     Scene Graph
Link                →     SceneNode / Part
Joint               →     Mechanical Joint
TF                  →     glm::mat4 Transform
joint_states        →     ExcavatorState
Gazebo Model        →     CET200 Model
RViz                →     3D HMI Viewer
```

因此后续 Scene Graph 和 Transform 不需要当成完全陌生体系理解。

---

# 19. 后续路线

推荐严格按以下顺序推进：

```text
A. 当前稳定 Viewer
        ↓
B. Model Matrix / Transform System
        ↓
C. SceneNode / Scene Graph
        ↓
D. 加载 CET200 独立 parts
        ↓
E. 读取 CET200 URDF/Xacro Joint 定义
        ↓
F. Upper → Boom → Arm → Bucket 层级
        ↓
G. 键盘/Slider 驱动关节
        ↓
H. ExcavatorState
        ↓
I. Qt HMI Shell
        ↓
J. CAN Simulator
        ↓
K. CAN Parser
        ↓
L. Real CAN
        ↓
M. Camera / Radar / Perception Overlay
```

文件选择器、Texture/PBR、阴影等 Viewer 功能可以后补，不要阻塞机械主线。

---

# 20. 下一会话恢复指令

新会话可以直接告诉模型：

> 我正在继续开发 `Construction3DHMI`。当前已经完成基于 C++17 + OpenGL 3.3 + GLFW + GLAD2 + GLM + Assimp 的最小 3D Viewer，CET200 GLB 可以正确加载，Material BaseColor/Metallic/Roughness 已读取，Orbit/Pan/Zoom/Fit、Bounding Box、Grid、XYZ Axis 已完成。当前稳定 Viewer 使用 Y-Up，因为 CET200 GLB 在现有加载链下 Y-Up 才正常站立；机械/URDF 侧 Z-Up 需要后续显式坐标转换。现在不要重做 Viewer，也不要直接猜关节 Pivot。下一步从 Model Matrix / Transform System 开始，验证外部 transform 能正确驱动整机，然后再实现 SceneNode 和 CET200 `upper_structure → boom → arm → bucket` 层级，并结合 URDF/Xacro 的 joint origin/axis 做正确机械运动。

---

## 21. 当前一句话状态

**Construction3DHMI 已完成稳定的 CET200 3D Viewer 底座，下一开发节点是 Model Transform → Scene Graph → CET200 机械层级，而不是继续扩展 Viewer 外观。**
