# Run this: powershell -ExecutionPolicy Bypass -File ..\buildAndLaunch.ps1 in the build directory

cmake -A Win32 ..
cmake --build . --config Release
Copy-Item ..\lib\glut32.dll Release\
Copy-Item ..\lib\glew32.dll Release\
# Change to Release directory so textures are found
Set-Location Release
.\FirstOGL.exe
