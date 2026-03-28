#!/bin/bash
# Zenith Open Source Projects — devpulse dependency installer
set -e

echo "[devpulse] Detecting Linux distro..."

if command -v apt &> /dev/null; then
    echo "[devpulse] Debian/Ubuntu detected"
    sudo apt update
    sudo apt install -y git cmake g++ gcc python3 python3-pip \
        libftxui-dev nlohmann-json3-dev valgrind gdb clang-format
elif command -v pacman &> /dev/null; then
    echo "[devpulse] Arch Linux detected"
    sudo pacman -Sy --noconfirm git cmake gcc python python-pip \
        nlohmann-json valgrind gdb clang
else
    echo "[devpulse] ERROR: Unknown distro. Install manually."
    exit 1
fi

# Install FTXUI from source if not found
if ! dpkg -l | grep -q libftxui 2>/dev/null && \
   ! pacman -Q ftxui 2>/dev/null; then
    echo "[devpulse] Installing FTXUI from source..."
    git clone https://github.com/ArthurSonzogni/FTXUI /tmp/ftxui
    cd /tmp/ftxui
    mkdir build && cd build
    cmake .. -DFTXUI_BUILD_EXAMPLES=OFF -DFTXUI_BUILD_TESTS=OFF
    make -j$(nproc)
    sudo make install
    cd ~
    rm -rf /tmp/ftxui
    echo "[devpulse] FTXUI installed successfully"
fi

# Install Python dependencies
pip3 install gitpython

echo "[devpulse] All dependencies installed successfully!"
