# build.ps1
Clear-Host
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host "  Building Visual Algorithm Simulator" -ForegroundColor Cyan
Write-Host "=====================================" -ForegroundColor Cyan
Write-Host ""

# Cek SFML
if (-not (Test-Path "C:/SFML")) {
    Write-Host "❌ SFML not found at C:/SFML" -ForegroundColor Red
    Write-Host "   Please download SFML and extract to C:/SFML" -ForegroundColor Yellow
    exit 1
}

# Bersihkan build
Write-Host "📁 Cleaning build directory..." -ForegroundColor Yellow
if (Test-Path "build") {
    Remove-Item -Recurse -Force "build"
}
New-Item -ItemType Directory -Path "build" -Force | Out-Null

# Configure CMake
Write-Host "🔧 Configuring CMake..." -ForegroundColor Yellow
Set-Location build
cmake .. -G "MinGW Makefiles"

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ CMake configuration failed!" -ForegroundColor Red
    exit 1
}

# Build
Write-Host "🔨 Building project..." -ForegroundColor Yellow
mingw32-make

if ($LASTEXITCODE -ne 0) {
    Write-Host "❌ Build failed!" -ForegroundColor Red
    exit 1
}

# Copy DLL
Write-Host "📋 Copying DLLs..." -ForegroundColor Yellow
Copy-Item "C:/SFML/bin/*.dll" -Destination "bin/" -Force

# Run
Write-Host ""
Write-Host "✅ Build successful!" -ForegroundColor Green
Write-Host "🚀 Running application..." -ForegroundColor Green
Write-Host ""

Set-Location bin
.\VisualAlgorithmSimulator.exe