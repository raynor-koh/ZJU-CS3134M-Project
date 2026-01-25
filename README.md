# ZJU-CS3134M-Project

## 项目简介

这是一个基于 OpenGL 的 3D 射击游戏项目，为浙江大学 CS3134M 课程开发。项目实现了完整的游戏系统，包括三种相机模式、敌人AI系统、玩家生命系统、碰撞检测、3D建模与导入导出、交互式光照系统等功能。

## 功能特性

### 核心游戏系统

1. **三种相机模式**
   - 第一人称视角（FPS模式）
   - 第三人称视角（跟随模式）
   - 自由相机/观察者模式（支持轨道旋转、平移、缩放）

2. **射击系统**
   - 子弹物理模拟
   - 可射击的靶子目标
   - 命中检测与伤害系统

3. **敌人AI系统**
   - 自动生成敌人（雪人造型）
   - 智能寻路与障碍物避让
   - 多射线检测的路径规划
   - 玩家追踪与攻击

4. **玩家系统**
   - 生命值管理（100HP）
   - 受伤与回血机制
   - 游戏结束判定

5. **碰撞检测**
   - 基于分离轴定理（SAT）的精确碰撞检测
   - 支持多种形状：圆柱体、球体、立方体
   - 边界墙碰撞

### 3D建模功能

1. **基础图元**
   - 立方体（Cube）
   - 球体（Sphere）
   - 圆柱体（Cylinder）
   - 圆锥体（Cone）
   - 棱柱（Prism）
   - 棱台（Frustum）

2. **网格导入/导出**
   - OBJ格式文件导入
   - 单个形状导出（支持世界坐标/局部坐标）
   - 整个场景导出

3. **编辑模式**
   - 物体选择与高亮显示
   - 材质颜色编辑（RGB滑块）
   - 纹理应用支持

### 光照系统

- OpenGL光照模型（环境光、漫反射、镜面反射）
- 点光源/平行光切换
- 光照强度调节
- 光源位置控制
- 头灯模式（跟随相机）

### 录制功能

- 基于FFmpeg的屏幕录制（MP4格式）
- 截图功能（PNG格式）
- 60 FPS流畅录制

## 项目结构

```
ZJU-CS3134M-Project/
├── include/                    # 头文件目录
│   ├── Camera.h               # 第一人称相机
│   ├── camera_controller.h    # 第三人称相机控制器
│   ├── FreeCamera.h           # 自由相机/观察者模式
│   ├── Player.h               # 玩家类
│   ├── Enemy.h                # 敌人类
│   ├── EnemyManager.h         # 敌人管理器
│   ├── GameObject.h           # 游戏对象基类
│   ├── Scene.h                # 场景管理
│   ├── InputHandler.h         # 输入处理
│   ├── Bullet.h               # 子弹类
│   ├── Target.h               # 靶子类
│   ├── Shapes.h               # 基础图元形状
│   ├── GenericMesh.h          # 通用网格
│   ├── MeshIO.h               # 网格导入导出
│   ├── CollisionDetector.h    # SAT碰撞检测
│   ├── Lighting.h             # 光照系统
│   ├── Texture.h              # 纹理管理
│   ├── UI.h                   # 用户界面
│   ├── ScreenRecorder.h       # 屏幕录制
│   ├── GameState.h            # 游戏状态枚举
│   └── Vector3.h              # 向量工具类
├── src/                        # 源代码目录
│   ├── main.cpp               # 主程序入口
│   ├── Camera.cpp             # 第一人称相机实现
│   ├── camera_controller.cpp  # 第三人称相机实现
│   ├── FreeCamera.cpp         # 自由相机实现
│   ├── Player.cpp             # 玩家实现
│   ├── Enemy.cpp              # 敌人实现
│   ├── EnemyManager.cpp       # 敌人管理器实现
│   ├── GameObject.cpp         # 游戏对象实现
│   ├── Scene.cpp              # 场景管理实现
│   ├── InputHandler.cpp       # 输入处理实现
│   ├── Bullet.cpp             # 子弹实现
│   ├── Target.cpp             # 靶子实现
│   ├── Shapes.cpp             # 基础图元实现
│   ├── GenericMesh.cpp        # 通用网格实现
│   ├── MeshIO.cpp             # 网格IO实现
│   ├── CollisionDetector.cpp  # 碰撞检测实现
│   ├── Lighting.cpp           # 光照实现
│   ├── Texture.cpp            # 纹理实现
│   ├── UI.cpp                 # UI实现
│   └── ScreenRecorder.cpp     # 录制实现
├── lib/                        # 库文件
│   ├── glut32.lib / glut32.dll
│   └── glew32.lib / glew32.dll
├── external/                   # 外部依赖
│   └── ffmpeg/                # FFmpeg（可选，用于录制）
├── resources/                  # 资源文件
│   ├── textures/              # 纹理图片
│   └── meshes/                # 3D模型文件
│       ├── imported/          # 导入的模型
│       └── exported/          # 导出的模型
├── videos/                     # 录制的视频
├── pics/                       # 截图
├── CMakeLists.txt             # CMake配置
├── buildAndLaunch.ps1         # 构建运行脚本
└── README.md                  # 项目说明
```

## 编译和运行

### 环境要求

- Windows操作系统
- CMake 3.10+
- Visual Studio（支持C++17）
- OpenGL
- GLUT和GLEW库（已包含在lib目录）
- FFmpeg（可选，用于屏幕录制）

### 快速构建（推荐）

```powershell
powershell -ExecutionPolicy Bypass -File .\buildAndLaunch.ps1
```

脚本会自动完成：配置CMake → 编译项目 → 复制DLL → 复制资源 → 启动程序

### 手动编译

```bash
mkdir build && cd build
cmake -A Win32 ..
cmake --build . --config Release
copy ..\lib\*.dll Release\
xcopy ..\resources Release\resources\ /E /I
cd Release && .\FirstOGL.exe
```

## 控制说明

### 基础控制

| 按键 | 功能 |
|------|------|
| W/A/S/D | 移动（前/左/后/右） |
| 空格 | 跳跃 |
| 鼠标移动 | 控制视角 |
| 鼠标左键 | 射击 |
| Tab | 切换鼠标捕获模式 |
| ESC | 退出编辑模式/退出游戏 |

### 相机控制

| 按键 | 功能 |
|------|------|
| C | 切换第一/第三人称视角 |
| V | 切换自由相机/观察者模式 |
| 鼠标滚轮 | 缩放（自由相机） |
| Alt+左键拖拽 | 轨道旋转（自由相机） |
| Alt+右键拖拽 | 平移（自由相机） |
| F | 适应窗口缩放（自由相机） |
| +/- | 键盘缩放（自由相机） |

### 编辑模式

| 按键 | 功能 |
|------|------|
| E | 切换编辑模式 |
| 鼠标点击 | 选择物体/操作UI |

### 光照控制

| 按键 | 功能 |
|------|------|
| L | 开关光照 |
| K | 切换点光源/平行光 |
| [ / ] | 调节光照强度 |
| 方向键 | 移动光源位置（X/Z轴） |
| Page Up/Down | 移动光源位置（Y轴） |

### 其他功能

| 按键 | 功能 |
|------|------|
| R | 开始/停止屏幕录制 |
| P | 截图 |
| T | 切换纹理 |

## 类设计概述

### 核心架构

项目采用**实体-组件模式**，以`GameObject`为基类构建对象层次结构，`Scene`作为中央管理器协调所有游戏实体、渲染和物理更新。

### 主要类

- **GameObject**: 所有场景实体的基类，包含位置、大小、颜色、碰撞类型
- **Scene**: 中央游戏管理器，管理所有实体集合和游戏循环
- **Camera/CameraController/FreeCamera**: 三种相机模式的实现
- **Player**: 玩家类，包含生命值系统和物理
- **Enemy/EnemyManager**: 敌人实体和AI控制器
- **CollisionDetector**: 基于SAT的碰撞检测
- **Shapes**: 基础图元（Cube、Sphere、Cylinder、Cone、Prism、Frustum）
- **MeshIO**: OBJ格式网格导入导出
- **Lighting**: 交互式光照系统
- **UI**: HUD、编辑界面、游戏结束界面

### 游戏状态

```cpp
enum GameState { PLAYING, PAUSED, GAME_OVER };
```

- **PLAYING**: 正常游戏，敌人生成和更新
- **PAUSED**: 自由相机或编辑模式，物理冻结
- **GAME_OVER**: 玩家死亡，显示结束界面

## 技术规格

- **平台**: Windows 32位
- **C++标准**: C++17
- **图形库**: OpenGL（固定管线）
- **工具库**: GLUT, GLEW
- **视频编码**: FFmpeg（可选）
- **构建系统**: CMake

## 课程要求完成情况

| 要求 | 状态 | 说明 |
|------|------|------|
| 建模 | ✅ | 6种基础图元全部实现 |
| 存储 | ✅ | OBJ格式导入导出 |
| 编辑 | ✅ | 编辑模式+材质编辑器 |
| 变换 | ⚠️ | API存在，交互控制待完善 |
| 光照 | ✅ | 完整光照系统+交互控制 |
| 漫游 | ✅ | 自由相机完整实现 |
| 记录 | ✅ | 屏幕录制+截图 |

## 许可证

本项目仅用于浙江大学 CS3134M 课程学习。