# Run this from the project root: powershell -ExecutionPolicy Bypass -File .\buildAndLaunch.ps1

# Save the original directory
Push-Location

# Create build folder if it doesn't exist
if (-Not (Test-Path -Path "build")) {
    Write-Host "Creating build directory..."
    New-Item -ItemType Directory -Path "build"
}

# Change to build directory
Set-Location build

# Run CMake and build
cmake -A Win32 ..
cmake --build . --config Release

# Copy DLLs to Release folder
Copy-Item ..\lib\glut32.dll Release\
Copy-Item ..\lib\glew32.dll Release\
if (-Not (Test-Path -Path "Release\resources")) {
    Copy-Item ..\resources Release\ -Recurse
}

# Change to Release directory and run the executable
Set-Location Release
.\FirstOGL.exe

# Return to the original directory
Pop-Location
