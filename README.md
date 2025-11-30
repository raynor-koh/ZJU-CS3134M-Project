# ZJU-CS3134M-Project

## 项目简介

这是一个基于 OpenGL 的第三人称射击游戏（FPS）项目，使用 GLUT 和 GLEW 库实现。当前实现了基础的 3D 场景渲染和第一人称相机控制系统。

## 功能特性

### 已实现功能

1. **3D 场景渲染**
   - 平坦的地面（50x50 单位，绿色草地）
   - 多个不同颜色和大小的立方体物体
   - 基础光照系统（环境光和漫反射光）
   - 天空蓝色背景

2. **第一人称相机系统**
   - 自由移动（WASD 控制）
   - 鼠标视角控制（yaw 和 pitch 旋转）
   - 平滑的移动和旋转
   - 可调节的移动速度和鼠标灵敏度

3. **输入系统**
   - WASD 键位移动
   - 鼠标控制视角
   - Tab 键切换鼠标捕获模式
   - ESC 退出程序

4. **鼠标捕获切换**
   - 游戏模式：鼠标被捕获，光标隐藏，用于控制视角
   - 自由模式：鼠标释放，光标可见，可以操作其他窗口

## 项目结构

```
ZJU-CS3134M-Project/
├── include/              # 头文件目录
│   ├── Camera.h         # 相机类
│   ├── GameObject.h     # 游戏对象类
│   ├── Scene.h          # 场景管理类
│   └── InputHandler.h   # 输入处理类
│   └── Texture.h        # 纹理类
│   └── Stob.h           # 碰撞物体类
├── src/                  # 源代码目录
│   ├── main.cpp         # 主程序入口
│   ├── Camera.cpp       # 相机实现
│   ├── GameObject.cpp   # 游戏对象实现
│   ├── Scene.cpp        # 场景管理实现
│   └── InputHandler.cpp # 输入处理实现
│   └── Texture.cpp      # 纹理实现
│   └── Stob.cpp         # 碰撞物体实现
├── lib/                  # 库文件
│   ├── glut32.lib
│   ├── glut32.dll
│   ├── glew32.lib
│   └── glew32.dll
├── build/                # 构建目录（自动生成）
├── CMakeLists.txt        # CMake 配置文件
├── buildAndLaunch.ps1    # 构建和运行脚本
└── README.md             # 项目说明文档
```

## 类设计说明

### Camera（相机类）
- **职责**：管理第一人称相机的位置、旋转和视图矩阵
- **主要方法**：
  - `move(forward, right)` - 相对于视角方向移动
  - `rotate(deltaYaw, deltaPitch)` - 旋转相机视角
  - `applyView()` - 应用相机视图变换

### GameObject（游戏对象类）
- **职责**：表示场景中的 3D 物体
- **属性**：位置（Vector3）、大小（Vector3）、颜色（Color）
- **主要方法**：
  - `draw()` - 渲染物体
  - 支持设置和获取位置、大小、颜色

### Scene（场景类）
- **职责**：管理游戏世界中的所有对象
- **主要方法**：
  - `addGameObject()` - 添加游戏对象
  - `draw()` - 渲染整个场景
  - `initialize()` - 初始化默认场景对象

### InputHandler（输入处理类）
- **职责**：处理键盘和鼠标输入
- **主要方法**：
  - `handleKeyPress()` / `handleKeyRelease()` - 键盘事件
  - `handleMouseMotion()` - 鼠标移动事件
  - `toggleMouseCapture()` - 切换鼠标捕获模式
  - `update()` - 更新相机移动

### Texture（纹理类）
- **职责**：管理纹理对象
- **属性**：ID（unsigned int）
- **主要方法**：
  - `rebind(filename)` - 更改纹理文件
  - 需输入.bmp格式图片

### Stob（碰撞物体类）
- **职责**：作为GameObject的子类，表示测试子弹的木桩
- **属性**：直径（float）、颜色（Color）、纹理（vector<Texture>）、圆柱切片数（int）
- **主要方法**：
  - `draw()` - 渲染木桩
  - `testCollision(posBullet, rBullet)` - 测试子弹碰撞
  - 支持设置颜色、切片数、是否加载纹理以及更改纹理图片

## 编译和运行

### 环境要求
- CMake 3.10 或更高版本
- Visual Studio（支持 C++17）
- OpenGL
- GLUT 和 GLEW 库（已包含在 lib 目录）

### 使用 PowerShell 脚本（推荐）

在项目根目录运行：
```powershell
powershell -ExecutionPolicy Bypass -File .\buildAndLaunch.ps1
```

脚本会自动：
1. 创建 build 目录（如果不存在）
2. 运行 CMake 配置
3. 编译项目
4. 复制必要的 DLL 文件
5. 启动程序

### 手动编译

```bash
# 创建并进入构建目录
mkdir build
cd build

# 配置 CMake（32位）
cmake -A Win32 ..

# 编译（Release 模式）
cmake --build . --config Release

# 复制 DLL
copy ..\lib\glut32.dll Release\
copy ..\lib\glew32.dll Release\

或者手动复制need文件夹下所有文件到 Release 目录

# 运行程序
cd Release
.\FirstOGL.exe
```

## 控制说明

| 按键 | 功能 |
|------|------|
| W | 向前移动 |
| A | 向左移动 |
| S | 向后移动 |
| D | 向右移动 |
| 鼠标移动 | 控制视角方向 |
| Tab | 切换鼠标捕获模式（释放/捕获光标） |
| ESC | 退出程序 |

## 扩展开发指南

### 添加新的游戏对象

在场景初始化或运行时添加：
```cpp
scene->addGameObject(new GameObject(
    Vector3(x, y, z),        // 位置
    Vector3(w, h, d),        // 大小
    Color(r, g, b)           // 颜色（0.0-1.0）
));
```

### 创建自定义对象类型

继承 `GameObject` 类：
```cpp
class Enemy : public GameObject {
public:
    void update();           // 更新敌人行为
    void takeDamage(int hp); // 受到伤害
private:
    int health;
};
```

### 修改相机参数

```cpp
camera->setMoveSpeed(0.3f);              // 设置移动速度
inputHandler->setMouseSensitivity(0.2f); // 设置鼠标灵敏度
```

### 调整场景参数

```cpp
scene->setGroundSize(100.0f);                    // 设置地面大小
scene->setGroundColor(Color(0.5f, 0.5f, 0.5f)); // 设置地面颜色
```

## 技术栈

- **语言**：C++17
- **图形库**：OpenGL
- **工具库**：GLUT (OpenGL Utility Toolkit)
- **扩展加载**：GLEW (OpenGL Extension Wrangler)
- **构建系统**：CMake

## 代码架构优势

1. **模块化设计**：各功能分离到独立的类中
2. **易于扩展**：基于继承和组合的设计模式
3. **可维护性**：清晰的代码结构和职责划分
4. **团队协作**：不同成员可以并行开发不同模块
5. **敏捷开发**：支持快速迭代和功能添加

## 许可证

本项目仅用于浙江大学 CS3134M 课程学习。
