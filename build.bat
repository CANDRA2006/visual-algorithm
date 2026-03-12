@echo off
echo 
echo  Visual Algorithm
echo 

if not exist "build" mkdir build
cd build

cmake .. -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed!
    pause
    exit /b 1
)

cmake --build . --config Release
if %errorlevel% neq 0 (
    echo [ERROR] Build failed!
    pause
    exit /b 1
)

echo.
echo [SUCCESS] Build complete!
echo Executable: build\bin\VisualAlgorithmSimulator.exe
pause
