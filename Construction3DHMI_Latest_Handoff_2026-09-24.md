# Construction3DHMI --- 最新开发交接文档

**更新时间：2026-09-24**\
**用途：下次会话快速恢复开发上下文。**\
本文只保留当前稳定状态、工程架构、关键约定和后续开发主线；安装过程、工具链冲突、编译
Bug、试错过程均不保留。

------------------------------------------------------------------------

## 1. 项目目标

项目名称：`Construction3DHMI`

目标是基于 CET200 工程机械模型，逐步构建一个面向工程机械的实时三维 HMI /
数字孪生显示底座。

最终目标：

``` text
CET200 / 工程机械模型
        ↓
Qt6 Industrial HMI
        ↓
实时 3D Viewer
        ↓
VehicleState 数据中枢
        ↓
SceneGraph / Joint / Kinematics
        ↓
CAN / 传感器 / Alarm / Scenario
        ↓
Camera / 360° Surround View
        ↓
Embedded Deployment
```

当前测试模型：CET200 挖掘机。

------------------------------------------------------------------------

## 2. 当前统一开发工具链

项目以后只使用 **MSYS2 UCRT64** 工具链，不再混用旧 MinGW。

``` text
Windows 11
VS Code
C++17
OpenGL 3.3 Core

C:\msys64\ucrt64
├── GCC / G++ 15.2
├── CMake
├── Ninja
├── Qt6
└── Assimp
```

关键原则：

``` text
只使用 C:\msys64\ucrt64
不要再使用 C:\Users\Admin\mingw64
```

当前第三方依赖：

``` text
Qt6
GLAD2
GLM
Assimp
```

GLFW 已不再承担主窗口生命周期；Qt6 已接管 Window / Layout / Input /
HMI。

------------------------------------------------------------------------

## 3. 当前已经完成的总体架构

当前程序已经从旧的 GLFW 3D Viewer 迁移到 Qt6 HMI：

``` text
QApplication
     ↓
MainWindow
     ↓
┌──────────────────────────────────────────┐
│ Top Status Bar                           │
├──────────────────────────────────────────┤
│ QStackedWidget                           │
│ ├── HOME                                 │
│ ├── VEHICLE                              │
│ ├── ENERGY                               │
│ ├── STATUS                               │
│ └── SETTINGS                             │
├──────────────────────────────────────────┤
│ Bottom Navigation                        │
└──────────────────────────────────────────┘
```

HOME 页内部：

``` text
┌────────────────────────────────────────────────────┐
│ CET200                     SYSTEM NORMAL     TIME  │
├──────────────┬─────────────────────┬───────────────┤
│              │                     │               │
│  SOC Gauge   │     CET200 3D       │  RPM Gauge    │
│              │   QOpenGLWidget     │               │
│              │                     │               │
├──────────────┴──────────┬──────────┴───────────────┤
│ COOLANT                 │ HYDRAULIC │ WORK MODE    │
├────────────────────────────────────────────────────┤
│ HOME | VEHICLE | ENERGY | STATUS | SETTINGS        │
└────────────────────────────────────────────────────┘
```

------------------------------------------------------------------------

## 4. Qt6 HMI 当前稳定状态

已完成：

-   `QApplication`
-   `MainWindow`
-   `QOpenGLWidget`
-   `QStackedWidget`
-   Top Status Bar
-   Bottom Navigation
-   HOME / VEHICLE / ENERGY / STATUS / SETTINGS 页面切换
-   HOME 页面完整布局
-   VEHICLE / ENERGY / STATUS / SETTINGS 当前为占位页

底部导航已经真正可切换页面，而不是静态按钮。

当前页面索引：

``` text
0 → HOME
1 → VEHICLE
2 → ENERGY
3 → STATUS
4 → SETTINGS
```

------------------------------------------------------------------------

## 5. CET200 已成功迁移进入 Qt

现有 CET200 Renderer 已经成功迁移进入 `QOpenGLWidget`。

当前渲染链：

``` text
Qt QOpenGLWidget
      ↓
QOpenGLContext
      ↓
GLAD2
      ↓
OpenGL 3.3 Core
      ↓
Model / Mesh / Material
      ↓
CET200
```

GLAD 不再通过 GLFW 加载 OpenGL 函数，而是通过当前 Qt OpenGL Context 的
`getProcAddress()` 加载。

当前 CET200 已经可以在 Qt HOME 页面中央正常显示。

保留的 Viewer 功能：

``` text
Orbit
Pan
Zoom
Fit
Bounding Box
Grid
XYZ Axis
Material
```

------------------------------------------------------------------------

## 6. 当前 3D Viewer 关键约定

### 6.1 Viewer 仍然使用 Y-Up

当前稳定 Viewer：

``` cpp
constexpr UpAxis VIEWER_UP_AXIS = UpAxis::Y;
```

原因：CET200 GLB 在当前 Assimp/OpenGL 加载链中使用 Y-Up 才能正常站立。

不要因为机器人/URDF 一侧通常是 Z-Up，就直接修改 Viewer Camera 或 Viewer
Up Axis。

必须保持：

``` text
GLB Model Space
        ≠
Robot / URDF Mechanical Space
```

未来正确方式：

``` text
Model Space
    ↓
Explicit Coordinate Conversion
    ↓
Mechanical / Scene Space
    ↓
Renderer
```

### 6.2 不允许通过 Camera 修正机械坐标

Camera 只负责观察。

不要使用 Camera 去修正：

-   GLB 坐标系
-   URDF 坐标系
-   Joint Axis
-   Joint Origin

### 6.3 不允许猜 Joint Pivot

未来 Boom / Arm / Bucket 运动必须读取真实机械定义：

``` text
joint origin xyz
joint origin rpy
joint axis
parent
child
```

不能凭视觉猜旋转中心。

------------------------------------------------------------------------

## 7. VehicleState 数据中枢

已经建立 `VehicleState`。

当前状态字段：

``` text
SOC
Engine RPM
Coolant Temperature
Hydraulic Oil Temperature
Work Mode
```

当前数据架构：

``` text
VehicleSimulator
       ↓
VehicleState
       ↓ Qt Signals
       ├── HMI Gauges
       ├── Status Cards
       └── 后续 3D / Alarm / Scenario
```

核心原则：

``` text
UI 不直接解析 CAN
OpenGL 不直接解析 CAN
CAN / Simulator / Sensor 不直接操作 UI
```

统一通过：

``` text
Input Source
     ↓
VehicleState
     ↓
Consumers
```

后续真实 CAN 接入时，不推翻 HMI，只替换/增加 VehicleState 的数据生产端。

------------------------------------------------------------------------

## 8. VehicleSimulator 当前状态

已经建立 `VehicleSimulator`。

当前用于模拟：

-   Engine RPM 动态变化
-   Coolant Temperature 缓慢变化
-   Hydraulic Oil Temperature 缓慢变化

Simulator 通过 Qt Timer 周期更新 `VehicleState`。

因此当前已经形成完整动态链：

``` text
VehicleSimulator
      ↓
VehicleState
      ↓
Qt Signal
      ↓
CircularGauge / QLabel
      ↓
实时 HMI
```

------------------------------------------------------------------------

## 9. CircularGauge 动态电子仪表

原来的矩形 SOC / RPM 卡片已经升级为汽车电子仪表风格的 `CircularGauge`。

`CircularGauge` 是自定义 `QWidget + QPainter` 控件。

当前视觉能力：

``` text
270° 主圆弧
外圈刻度
Major Tick
Minor Tick
数字刻度
Active Arc
动态指针
中央大数字
单位
标题
```

当前动态能力：

``` text
VehicleState Value
       ↓
CircularGauge::setValue()
       ↓
QVariantAnimation
       ↓
displayValue
       ↓
QPainter repaint
```

RPM 变化时：

-   中央数字平滑变化
-   指针平滑扫动
-   Active Arc 平滑变化

当前仪表配置：

### SOC

``` text
Range: 0 ~ 100
Unit: %
Major Tick Count: 6
Minor Ticks Per Major: 4
```

### ENGINE

``` text
Range: 0 ~ 2500
Unit: r/min
Major Tick Count: 6
Minor Ticks Per Major: 4
```

------------------------------------------------------------------------

## 10. HOME 页面当前布局修正版

最新稳定布局中，`createHomePage()` 对主区域进行了明确 SizePolicy 管理。

关键约定：

### mainArea

``` text
Horizontal: Expanding
Vertical:   Expanding
```

并通过：

``` cpp
root->addWidget(mainArea, 1);
```

让主显示区获得剩余垂直空间。

### SOC / RPM Gauge

当前建议：

``` text
Minimum Size: 250 × 250
Maximum Width: 330
Horizontal Policy: Preferred
Vertical Policy: Expanding
```

### OpenGLWidget

当前建议：

``` text
Minimum Size: 500 × 300
Horizontal Policy: Expanding
Vertical Policy: Expanding
```

布局：

``` text
SOC Gauge
   +
OpenGLWidget (stretch = 1)
   +
RPM Gauge
```

这一布局已经验证可以正常同时显示：

``` text
SOC Gauge + CET200 + RPM Gauge
```

------------------------------------------------------------------------

## 11. 当前主要源文件

当前工程核心代码至少包括：

``` text
src/
├── main.cpp
├── MainWindow.h
├── MainWindow.cpp
├── OpenGLWidget.h
├── OpenGLWidget.cpp
├── CircularGauge.h
├── CircularGauge.cpp
├── VehicleState.h
├── VehicleState.cpp
├── VehicleSimulator.h
├── VehicleSimulator.cpp
├── Model.h
├── Model.cpp
├── Mesh.h
├── Mesh.cpp
├── Material.h
├── OrbitCamera.h
├── OrbitCamera.cpp
├── GridRenderer.h
├── GridRenderer.cpp
├── AxisRenderer.h
└── AxisRenderer.cpp
```

------------------------------------------------------------------------

## 12. main.cpp 当前职责

`main.cpp` 保持简单，不放业务逻辑。

职责：

``` text
QSurfaceFormat
      ↓
OpenGL 3.3 Core
      ↓
QApplication
      ↓
MainWindow
```

不要在 `main.cpp` 创建：

``` text
VehicleState
VehicleSimulator
OpenGLWidget
CircularGauge
```

这些由 MainWindow / 页面内部管理。

------------------------------------------------------------------------

## 13. MainWindow 当前职责

MainWindow 当前负责：

``` text
Application HMI Shell
Page Navigation
Top Bar
Bottom Navigation
HOME Page Composition
VehicleState Binding
VehicleSimulator Lifecycle
```

当前包含：

``` text
VehicleState*
VehicleSimulator*

QStackedWidget*

HOME / VEHICLE / ENERGY / STATUS / SETTINGS Buttons

CircularGauge* socGauge_
CircularGauge* rpmGauge_

QLabel* coolantValue_
QLabel* hydraulicValue_
QLabel* workModeValue_
```

------------------------------------------------------------------------

## 14. 当前 UI 开发方向

虽然长期主线包含 SceneGraph / Joint / Kinematics，但当前用户明确选择：

> 先继续完善 UI，不急于进入机械关节运动。

因此恢复开发时，不要立即跳入 SceneGraph。

当前近期优先级：

``` text
1. HOME 页工业 HMI 视觉继续升级
2. 顶部状态图标 / 档位 / 系统状态
3. 仪表视觉细化
4. 状态卡玻璃态 / 告警状态
5. 页面切换视觉
6. VEHICLE 页面
7. ENERGY 页面
8. STATUS / Alarm 页面
9. SETTINGS 页面
```

UI 稳定后再进入机械 3D。

------------------------------------------------------------------------

## 15. 后续机械主线

UI 阶段完成后，恢复长期技术主线：

``` text
Current Qt HMI + Stable CET200
        ↓
Transform System
        ↓
SceneGraph
        ↓
CET200 Semantic Parts
        ↓
URDF / Xacro Joint Definition
        ↓
Upper Structure
        ↓
Boom
        ↓
Arm
        ↓
Bucket
        ↓
Kinematics
        ↓
VehicleState-driven 3D
```

第一版机械层级目标：

``` text
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

------------------------------------------------------------------------

## 16. 后续 VehicleState 扩展

进入机械运动阶段后，可在 VehicleState 中逐步加入：

``` text
slewAngle
boomAngle
armAngle
bucketAngle
```

最终：

``` text
Simulator / CAN / ROS / Sensor
             ↓
        VehicleState
             ↓
        Kinematics
             ↓
         SceneGraph
             ↓
           3D HMI
```

Renderer 不关心数据来源。

------------------------------------------------------------------------

## 17. 后续完整项目路线

``` text
[已完成]
Qt6 HMI Shell
      ↓
CET200 Renderer → QOpenGLWidget
      ↓
HOME HMI Layout
      ↓
VehicleState
      ↓
VehicleSimulator
      ↓
Dynamic Circular Gauges
      ↓
QStackedWidget Navigation

[当前]
Industrial HMI UI Polish
      ↓
Vehicle / Energy / Status / Settings Pages

[后续]
Transform System
      ↓
SceneGraph
      ↓
Joint / Kinematics
      ↓
State-driven CET200
      ↓
CAN Simulator / Parser
      ↓
Real CAN / Sensors
      ↓
Alarm / Scenario
      ↓
Camera / 360°
      ↓
Embedded Deployment
```

------------------------------------------------------------------------

## 18. 开发原则

继续遵守：

1.  每一步必须可编译、可运行。
2.  可以适当迈大步，但一次修改应围绕一个明确目标。
3.  不做大爆炸式重构。
4.  不重复折腾已经稳定的开发环境。
5.  不重新退回 GLFW Window 主线。
6.  Qt 负责 HMI / Window / Layout / Navigation。
7.  Renderer 负责 OpenGL / Model / Camera / Scene。
8.  VehicleState 是数据中枢。
9.  CAN / Sensor 不直接操作 UI/OpenGL。
10. Viewer 保持 Y-Up。
11. URDF / Mechanical 坐标后续显式转换。
12. 不猜 Joint Pivot。

------------------------------------------------------------------------

## 19. 当前一句话状态

**Construction3DHMI 已经从独立 3D Viewer 进入可运行的 Qt6 工业 HMI
阶段：CET200 已稳定运行于 QOpenGLWidget，VehicleState + VehicleSimulator
已建立，HOME 页具备动态汽车电子式 SOC/RPM
仪表、实时状态卡和多页面导航；当前优先继续完善 HMI UI，之后再进入
SceneGraph / Joint / Kinematics。**

------------------------------------------------------------------------

## 20. 下一会话快速恢复指令

新会话可直接使用：

> 我正在继续开发 Construction3DHMI。当前统一使用 MSYS2 UCRT64
> 工具链（GCC/G++、CMake/Ninja、Qt6、Assimp），不要再使用旧
> MinGW。项目已经完成 Qt6 QApplication + MainWindow +
> QOpenGLWidget，CET200 已成功迁移到 HOME 页中央并保留
> Orbit/Pan/Zoom/Fit、Grid/Axis；Viewer 保持 Y-Up。已经建立 VehicleState
> 和 VehicleSimulator，SOC、RPM、冷却液温度、液压油温度、工作模式通过 Qt
> signals 驱动 HMI。左右 SOC/RPM 已升级为 QPainter + QVariantAnimation
> 的 270° 汽车电子式 CircularGauge，RPM
> 指针、数字和进度弧可平滑动态变化。MainWindow 已使用 QStackedWidget
> 实现 HOME/VEHICLE/ENERGY/STATUS/SETTINGS 导航。最新 HOME
> 布局已修复，SOC Gauge + CET200 + RPM Gauge
> 可稳定同时显示。当前先继续完善工业 HMI UI，不要立即进入 SceneGraph；UI
> 稳定后再按 Transform → SceneGraph → URDF Joint → Kinematics →
> State-driven 3D → CAN/Alarm/Camera
> 的主线推进。机械坐标必须显式转换，不通过 Camera 修正，也不猜 Joint
> Pivot。
