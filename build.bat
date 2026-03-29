@echo off
setlocal enabledelayedexpansion

echo ========================================
echo   DevPulse Windows Build Script
echo ========================================
echo.

REM Check for MSYS2/MinGW
where g++ >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] MinGW g++ not found!
    echo Please install MSYS2 from: https://www.msys2.org/
    echo Then run: pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
    exit /b 1
)

REM Check for CMake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] CMake not found!
    echo Please install CMake or use MSYS2: pacman -S mingw-w64-x86_64-cmake
    exit /b 1
)

REM Check for git
where git >nul 2>&1
if %errorlevel% neq 0 (
    echo [WARNING] Git not found. Git panel may not work.
    set "GIT_FLAG=-DFTXUI_FEATURE_GIT=OFF"
) else (
    set "GIT_FLAG=-DFTXUI_FEATURE_GIT=ON"
)

echo [INFO] Dependencies found.
echo.

REM Set MSYS2 paths if not already set
set "MSYS_PATH=C:\msys64\mingw64\bin"
if exist "%MSYS_PATH%" (
    set "PATH=%MSYS_PATH%;%PATH%"
)

REM Create build directory
echo [STEP 1] Creating build directory...
if exist build rmdir /s /q build
mkdir build
cd build
if %errorlevel% neq 0 (
    echo [ERROR] Failed to create build directory
    exit /b 1
)

REM Configure with CMake
echo [STEP 2] Configuring with CMake...
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_CXX_STANDARD=17 ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_EXE_LINKER_FLAGS="-static" ^
    %GIT_FLAG%
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed
    cd ..
    exit /b 1
)

REM Build
echo [STEP 3] Building devpulse.exe...
cmake --build . -- -j4
if %errorlevel% neq 0 (
    echo [ERROR] Build failed
    cd ..
    exit /b 1
)

cd ..

REM Check if executable was created
if not exist "build\devpulse.exe" (
    echo [WARNING] Executable not found at build\devpulse.exe
    dir build\*.exe
    goto :end
)

REM Copy required runtime DLLs for distribution
echo [STEP 4] Bundling runtime DLLs...
if not exist "dist\runtime" mkdir "dist\runtime"
copy /Y "%MSYS_PATH%\libgcc_s_seh-1.dll" "dist\runtime\" >nul 2>&1
copy /Y "%MSYS_PATH%\libstdc++-6.dll" "dist\runtime\" >nul 2>&1
copy /Y "%MSYS_PATH%\libwinpthread-1.dll" "dist\runtime\" >nul 2>&1
echo [INFO] Runtime DLLs copied to dist\runtime\

REM Copy executable to dist folder
copy /Y "build\devpulse.exe" "dist\" >nul 2>&1
echo [INFO] Executable copied to dist\

:end

echo.
echo ========================================
echo   Build Complete!
echo ========================================
echo.
echo Distribution folder: dist\
echo   - devpulse.exe
echo   - runtime\  (required DLLs)
echo.
echo To distribute: Copy the entire 'dist' folder.
echo.

exit /b 0
