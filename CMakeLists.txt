cmake_minimum_required(VERSION 3.10)

# 项目名称和语言
project(FirstOGL)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)

# 指定 include 目录
# 假设你的 glut.h 放在 include/GL/ 下面
include_directories(${CMAKE_SOURCE_DIR}/include)

# FFmpeg include directory (update this path to where you install FFmpeg)
include_directories(${CMAKE_SOURCE_DIR}/external/ffmpeg/include)

# 指定 lib 目录
# 假设 glut32.lib 放在 lib/ 下面
link_directories(${CMAKE_SOURCE_DIR}/lib)

# FFmpeg library directory (update this path to where you install FFmpeg)
link_directories(${CMAKE_SOURCE_DIR}/external/ffmpeg/lib)

# 源代码
add_executable(FirstOGL
    src/main.cpp
    src/Camera.cpp
    src/GameObject.cpp
    src/Scene.cpp
    src/InputHandler.cpp
    src/Stob.cpp
    src/Texture.cpp
    src/camera_controller.cpp
    src/UI.cpp
    src/ScreenRecorder.cpp
    src/Target.cpp
    src/Bullet.cpp
)

# 链接库
target_link_libraries(FirstOGL
    opengl32.lib   # Windows 自带 OpenGL 库
    glut32.lib     # 老师提供的 glut 库
    glew32.lib
    # FFmpeg libraries
    avcodec.lib
    avformat.lib
    avutil.lib
    swscale.lib
)

# 禁用 SAFESEH
set_target_properties(FirstOGL PROPERTIES LINK_FLAGS "/SAFESEH:NO")