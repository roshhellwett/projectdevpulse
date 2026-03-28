#!/bin/bash
# Zenith Open Source Projects — devpulse first time build
set -e

echo "[devpulse] Setting up build directory..."

# Clean any old build
if [ -d "build" ]; then
    echo "[devpulse] Old build found. Removing..."
    rm -rf build
fi

mkdir build
cd build

echo "[devpulse] Running CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Debug

echo "[devpulse] Building..."
make -j$(nproc)

echo ""
echo "[devpulse] Build successful!"
echo "[devpulse] Run ./run.sh to start devpulse"
