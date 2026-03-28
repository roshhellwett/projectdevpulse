#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "[devpulse] ============================================="
echo "[devpulse] devpulse build script"
echo "[devpulse] ============================================="

install_deps_debian() {
    echo "[devpulse] Debian/Ubuntu detected - installing system deps..."
    sudo apt update
    sudo apt install -y git cmake g++ gcc python3 python3-pip \
        nlohmann-json3-dev valgrind gdb clang-format
    pip3 install --break-system-packages gitpython 2>/dev/null || pip3 install gitpython
    echo "[devpulse] System deps installed"
}

install_deps_arch() {
    echo "[devpulse] Arch Linux detected - installing system deps..."
    sudo pacman -Sy --noconfirm git cmake gcc python python-pip \
        nlohmann-json valgrind gdb clang
    pip install gitpython 2>/dev/null || pip3 install gitpython
    echo "[devpulse] System deps installed"
}

install_ftxui() {
    if dpkg -l | grep -q libftxui 2>/dev/null; then
        echo "[devpulse] FTXUI already installed"
        return 0
    fi
    if pacman -Q ftxui &>/dev/null; then
        echo "[devpulse] FTXUI already installed"
        return 0
    fi
    echo "[devpulse] Installing FTXUI from source..."
    rm -rf /tmp/ftxui
    git clone --depth 1 https://github.com/ArthurSonzogni/FTXUI /tmp/ftxui
    cd /tmp/ftxui
    mkdir -p build && cd build
    cmake .. -DFTXUI_BUILD_EXAMPLES=OFF -DFTXUI_BUILD_TESTS=OFF -DFTXUI_BUILD_DOCS=OFF
    make -j$(nproc)
    sudo make install
    sudo ldconfig
    cd "$SCRIPT_DIR"
    rm -rf /tmp/ftxui
    echo "[devpulse] FTXUI installed successfully"
}

echo "[devpulse] Phase 1: Installing dependencies..."
if command -v apt &> /dev/null; then
    install_deps_debian
elif command -v pacman &> /dev/null; then
    install_deps_arch
else
    echo "[devpulse] ERROR: Unsupported distro. Install manually: git, cmake, g++, gcc, python3, python3-pip, nlohmann-json3-dev, valgrind, gdb, clang-format"
    exit 1
fi

echo "[devpulse] Phase 2: Installing FTXUI..."
install_ftxui

echo "[devpulse] Phase 3: Building devpulse..."
cd "$SCRIPT_DIR"
rm -rf build
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

echo ""
echo "[devpulse] ============================================="
echo "[devpulse] Build complete!"
echo "[devpulse] Run ./run.sh to launch devpulse"
echo "[devpulse] ============================================="
