#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "[devpulse] ============================================="
echo "[devpulse] devpulse run script"
echo "[devpulse] ============================================="

if [ ! -d "$SCRIPT_DIR/build" ]; then
    echo "[devpulse] No build found. Running build.sh first..."
    "$SCRIPT_DIR/build.sh"
fi

echo "[devpulse] Rebuilding..."
cd "$SCRIPT_DIR/build"
if ! make -j$(nproc); then
    echo ""
    echo "[devpulse] BUILD FAILED. Fix errors above then run ./run.sh again."
    exit 1
fi

echo ""
echo "[devpulse] Launching devpulse..."
echo "[devpulse] ============================================="
./devpulse
