#!/bin/bash
# Zenith Open Source Projects — devpulse run script
set -e

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "[devpulse] No build found. Running build.sh first..."
    ./build.sh
fi

echo "[devpulse] Rebuilding..."
cd build

# Build and capture errors
if ! make -j$(nproc); then
    echo ""
    echo "[devpulse] BUILD FAILED. Fix errors above then run ./run.sh again."
    exit 1
fi

echo "[devpulse] Launching devpulse..."
echo "-----------------------------------"
./devpulse
