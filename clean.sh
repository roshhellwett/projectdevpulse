#!/bin/bash

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

if [ "$1" == "--full" ] || [ "$1" == "-f" ]; then
    echo "[devpulse] Full clean selected..."
    echo "[devpulse] Removing build directory..."
    rm -rf "$SCRIPT_DIR/build"
    echo "[devpulse] Build directory cleaned"
    echo "[devpulse] Next run of ./build.sh will reinstall dependencies"
else
    echo "[devpulse] Cleaning build directory..."
    rm -rf "$SCRIPT_DIR/build"
    echo "[devpulse] Done. Run ./build.sh to rebuild."
fi
