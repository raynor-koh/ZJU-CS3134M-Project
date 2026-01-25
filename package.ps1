# Package script for ZJU CS3134M Project Submission
# Run: powershell -ExecutionPolicy Bypass -File .\package.ps1

$ErrorActionPreference = "Stop"

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  ZJU CS3134M Project Packaging Tool" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Define output folder name
$packageName = "ZJU-CS3134M-3D游戏项目"
$packagePath = ".\$packageName"
$zipPath = ".\$packageName.zip"

# Step 1: Build the project
Write-Host "[1/6] Building project..." -ForegroundColor Yellow

if (-Not (Test-Path -Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Push-Location build
cmake -A Win32 .. | Out-Null
cmake --build . --config Release
if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    Pop-Location
    exit 1
}
Pop-Location

Write-Host "Build successful!" -ForegroundColor Green

# Step 2: Create package folder structure
Write-Host "[2/6] Creating package folder structure..." -ForegroundColor Yellow

if (Test-Path -Path $packagePath) {
    Remove-Item -Path $packagePath -Recurse -Force
}

# Create folder structure
New-Item -ItemType Directory -Path $packagePath | Out-Null
New-Item -ItemType Directory -Path "$packagePath\1-可运行程序" | Out-Null
New-Item -ItemType Directory -Path "$packagePath\2-源代码" | Out-Null
New-Item -ItemType Directory -Path "$packagePath\3-设计文档" | Out-Null

Write-Host "Folder structure created!" -ForegroundColor Green

# Step 3: Copy executable and dependencies
Write-Host "[3/6] Copying executable and dependencies..." -ForegroundColor Yellow

$exePath = "$packagePath\1-可运行程序"

# Copy executable
Copy-Item "build\Release\FirstOGL.exe" $exePath

# Copy GLUT/GLEW DLLs
Copy-Item "lib\glut32.dll" $exePath
Copy-Item "lib\glew32.dll" $exePath

# Copy FFmpeg DLLs if available
if (Test-Path -Path "external\ffmpeg\bin") {
    Copy-Item "external\ffmpeg\bin\*.dll" $exePath -ErrorAction SilentlyContinue
    Write-Host "  - FFmpeg DLLs copied (screen recording enabled)" -ForegroundColor Gray
} else {
    Write-Host "  - FFmpeg not found (screen recording disabled)" -ForegroundColor Gray
}

# Copy resources folder
Copy-Item "resources" "$exePath\resources" -Recurse

# Create videos and pics folders
New-Item -ItemType Directory -Path "$exePath\videos" -ErrorAction SilentlyContinue | Out-Null
New-Item -ItemType Directory -Path "$exePath\pics" -ErrorAction SilentlyContinue | Out-Null

# Create launcher batch file
$launcherContent = @"
@echo off
chcp 65001 >nul
echo ========================================
echo   ZJU CS3134M 3D射击游戏
echo ========================================
echo.
echo 控制说明:
echo   WASD     - 移动
echo   鼠标     - 视角控制
echo   左键     - 射击
echo   空格     - 跳跃
echo   Tab      - 释放/捕获鼠标
echo   C        - 切换第一/第三人称
echo   V        - 自由相机模式
echo   E        - 编辑模式
echo   L        - 开关光照
echo   R        - 录制视频
echo   P        - 截图
echo   ESC      - 退出
echo.
echo 按任意键启动游戏...
pause >nul
FirstOGL.exe
"@
$launcherContent | Out-File -FilePath "$exePath\运行游戏.bat" -Encoding UTF8

Write-Host "Executable package ready!" -ForegroundColor Green

# Step 4: Copy source code
Write-Host "[4/6] Copying source code..." -ForegroundColor Yellow

$srcPath = "$packagePath\2-源代码"

# Copy source folders
Copy-Item "src" "$srcPath\src" -Recurse
Copy-Item "include" "$srcPath\include" -Recurse
Copy-Item "lib" "$srcPath\lib" -Recurse
Copy-Item "resources" "$srcPath\resources" -Recurse

# Copy build files
Copy-Item "CMakeLists.txt" $srcPath
Copy-Item "buildAndLaunch.ps1" $srcPath

# Copy documentation
Copy-Item "README.md" $srcPath
Copy-Item "CLAUDE.md" $srcPath -ErrorAction SilentlyContinue

# Create GitHub link file
$githubContent = @"
GitHub 仓库地址
================

请访问以下地址查看完整源代码和提交历史:

https://github.com/raynor-koh/ZJU-CS3134M-Project
"@
$githubContent | Out-File -FilePath "$srcPath\GitHub仓库地址.txt" -Encoding UTF8

Write-Host "Source code copied!" -ForegroundColor Green

# Step 5: Copy documentation
Write-Host "[5/6] Copying documentation..." -ForegroundColor Yellow

$docPath = "$packagePath\3-设计文档"

Copy-Item "README.md" $docPath
Copy-Item "系统设计说明文档.md" $docPath

# Create placeholder for demo video
$videoNote = @"
演示视频说明
============

请将演示视频文件放置在此目录下。

视频建议内容（约5分钟）:
1. 基础建模演示 - 展示6种图元和材质编辑
2. 场景漫游演示 - 自由相机的轨道/平移/缩放/适应窗口
3. 光照系统演示 - 开关光照、切换光源类型、移动光源
4. 游戏玩法演示 - 射击敌人、血量系统、游戏结束
5. 录制功能演示 - 视频录制和截图

推荐视频格式: MP4 (H.264)
推荐分辨率: 1920x1080 或 1280x720
"@
$videoNote | Out-File -FilePath "$docPath\演示视频说明.txt" -Encoding UTF8

Write-Host "Documentation copied!" -ForegroundColor Green

# Step 6: Create ZIP archive
Write-Host "[6/6] Creating ZIP archive..." -ForegroundColor Yellow

if (Test-Path -Path $zipPath) {
    Remove-Item -Path $zipPath -Force
}

Compress-Archive -Path $packagePath -DestinationPath $zipPath -CompressionLevel Optimal

Write-Host "ZIP archive created: $zipPath" -ForegroundColor Green

# Summary
Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Packaging Complete!" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Package folder: $packagePath" -ForegroundColor White
Write-Host "ZIP file:       $zipPath" -ForegroundColor Green
Write-Host ""
Write-Host "Folder structure:" -ForegroundColor White
Write-Host "  $packageName\" -ForegroundColor Yellow
Write-Host "  ├── 1-可运行程序\" -ForegroundColor Gray
Write-Host "  │   ├── FirstOGL.exe" -ForegroundColor Gray
Write-Host "  │   ├── 运行游戏.bat  (双击运行)" -ForegroundColor Green
Write-Host "  │   ├── *.dll" -ForegroundColor Gray
Write-Host "  │   └── resources\" -ForegroundColor Gray
Write-Host "  ├── 2-源代码\" -ForegroundColor Gray
Write-Host "  │   ├── src\" -ForegroundColor Gray
Write-Host "  │   ├── include\" -ForegroundColor Gray
Write-Host "  │   └── ..." -ForegroundColor Gray
Write-Host "  └── 3-设计文档\" -ForegroundColor Gray
Write-Host "      ├── README.md" -ForegroundColor Gray
Write-Host "      ├── 系统设计说明文档.md" -ForegroundColor Gray
Write-Host "      └── [演示视频.mp4]  (需手动添加)" -ForegroundColor Yellow
Write-Host ""
Write-Host "Next steps:" -ForegroundColor White
Write-Host "  1. Record demo video and add to 3-设计文档\ (then re-run this script)" -ForegroundColor Gray
Write-Host "  2. Upload $packageName.zip to Baidu Cloud" -ForegroundColor Green
Write-Host ""
