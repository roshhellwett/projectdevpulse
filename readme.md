# PROJECT DEVPULSE

<p align="left">
  <img src="https://img.shields.io/badge/Version-1.0.0-00d9ff?style=for-the-badge" alt="Version">
  <img src="https://img.shields.io/badge/License-MIT-ff6b6b?style=for-the-badge" alt="License">
  <img src="https://img.shields.io/badge/Platform-Windows%20x64-00ff88?style=for-the-badge" alt="Platform">
</p>

DevPulse is a **real-time terminal dashboard** for developers. It shows you everything important in one window:
- Your computer's CPU, RAM, and disk usage
- Git branch and changes in your project
- Developer tools running on your PC
- Your task list
- Application logs

No more switching between windows to check these things!

---

## Quick Start (5 Minutes)

### Step 1: Install Required Software

You need **two free programs** to run DevPulse:

#### 1.1 Install MSYS2
1. Go to: [https://www.msys2.org/](https://www.msys2.org/)
2. Click the blue **"Download"** button
3. Run the downloaded file (msys2-x86_64-*.exe)
4. Follow the installation wizard
5. **Important**: When it asks to run MSYS2 now, click **"Run"**

#### 1.2 Install Build Tools (in MSYS2 terminal)
A black window will open. Copy and paste this command, then press Enter:

```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make
```

Press `Y` if it asks to confirm. Wait for it to finish (might take a few minutes).

### Step 2: Download DevPulse

#### Option A: Download Ready-to-Run Version (Easiest)
1. Go to the [Releases page](https://github.com/roshhellwett/projectdevpulse/releases)
2. Download `DevPulse-v1.0.0.zip`
3. Extract the ZIP file
4. Double-click `devpulse.exe` to run!

#### Option B: Build From Source (For Developers)
1. Open **File Explorer**
2. Navigate to where you want the project
3. Right-click → **"Open Git Bash here"**
4. Run these commands:

```bash
git clone https://github.com/roshhellwett/projectdevpulse.git
cd projectdevpulse
```

5. Now run the build script:
```bash
./build.bat
```

6. Wait for "Build Complete!" message
7. Run DevPulse:
```bash
./dist/devpulse.exe
```

---

## How to Use DevPulse

### The Dashboard Layout

```
┌─────────────────────────────────────────────────────────────────────┐
│                        DEVPULSE  •  4 Panels                        │
├──────────────────────┬──────────────────────────────────────────────┤
│                      │                                              │
│   ┌─ SYSTEM ─────┐   │   ┌─ GIT STATUS ─────────────────────────┐   │
│   │ CPU: 45%     │   │   │ Branch: main  │ +2 -1 ~3            │   │
│   │ [████████░░] │   │   │                                    │   │
│   │              │   │   │ a1b2c3d Fix login bug              │   │
│   │ RAM: 8.2GB   │   │   │ e4f5g6h Add user profile           │   │
│   │ [██████░░░░] │   │   └────────────────────────────────────┘   │
│   │              │   │                                              │
│   │ Disk: 234GB  │   │   ┌─ LOG VIEWER ─────────────────────────┐   │
│   │ [████████░░] │   │   │ [10:30] [INF] Server started        │   │
│   └──────────────┘   │   │ [10:31] [OK]  User logged in        │   │
│                      │   │ [10:32] [WRN] Slow query detected   │   │
│   ┌─ PROCESSES ──┐   │   └────────────────────────────────────┘   │
│   │ gcc.exe  3x  │   │                                              │
│   │ node.exe 2x  │   │   ┌─ TASKS ────────────────────────────┐   │
│   │ code.exe 1x  │   │   │ [✓] Complete project setup         │   │
│   └──────────────┘   │   │ [ ] Write documentation            │   │
│                      │   │ [ ] Test user registration         │   │
│                      │   └────────────────────────────────────┘   │
└──────────────────────┴──────────────────────────────────────────────┘
```

### Keyboard Controls

| Key | Action |
|-----|--------|
| `q` | **Quit** - Close DevPulse |
| `a` | **Add task** - Type a new task, press Enter to save |
| `d` | **Toggle done** - Mark selected task as complete/incomplete |
| `x` | **Delete task** - Remove selected task |
| `↑` | **Move up** - Select previous task |
| `↓` | **Move down** - Select next task |
| `Tab` | **Switch panel** - Move between different panels |

### Understanding the Colors

| Color | Meaning |
|-------|---------|
| 🟢 Green | Everything is fine / Low usage |
| 🟡 Yellow | Warning / Medium usage |
| 🔴 Red | Alert / High usage |
| 🔵 Cyan | Information / Active |

---

## Troubleshooting

### "MinGW g++ not found!"
**Problem**: MSYS2 is not installed or not in PATH.

**Solution**:
1. Open Start Menu
2. Search for "MSYS2 MingGW 64-bit"
3. Click to open it
4. Type this command and press Enter:
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake
```

### "CMake configuration failed"
**Problem**: Missing build tools.

**Solution**:
```bash
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-make
```

### App crashes on startup
**Problem**: Missing DLL files.

**Solution**: Make sure all 4 files are in the same folder:
- `devpulse.exe`
- `libgcc_s_seh-1.dll`
- `libstdc++-6.dll`
- `libwinpthread-1.dll`

### Git panel shows "Not a git repo"
**Problem**: You're running DevPulse from a folder that's not a git project.

**Solution**: Navigate to a folder with a `.git` folder, or use the ready-made release version.

### Tasks don't save
**Problem**: DevPulse saves tasks to AppData.

**Solution**: Tasks are saved to `%APPDATA%\DevPulse\tasks.json`. This is normal behavior.

---

## Configuration

Edit `devpulse.toml` to customize behavior:

```toml
[general]
refresh_rate_ms = 1000   # How often to update (1000 = 1 second)
theme = "dark"          # Currently only dark theme supported

[panels]
show_system  = true     # Show CPU/RAM/Disk panel
show_git     = true     # Show Git status panel
show_process = true     # Show process tracker panel
show_log     = true     # Show log viewer panel
show_tasks   = true     # Show task manager panel

[log]
file = "devpulse.log"   # Log file to watch

[process]
filter = ["gcc", "g++", "clang", "clang++", "python", "node", "npm", "vim", "nvim", "make", "cmake", "clangd"]
```

---

## Building from Source (Advanced)

If you want to modify DevPulse:

### Requirements
- Windows 10/11 x64
- MSYS2 with MinGW-w64
- Git

### Build Steps
```bash
git clone https://github.com/roshhellwett/projectdevpulse.git
cd projectdevpulse
./build.bat
```

### Creating an Installer (Optional)
```bash
iscc setup.iss
```

---

## Project Structure

```
devpulse/
├── src/
│   ├── app.cpp/h          # Main application
│   ├── main.cpp          # Entry point
│   ├── core/
│   │   └── sysreader_win.cpp  # System statistics
│   ├── panels/
│   │   ├── system_panel.cpp   # CPU/RAM display
│   │   ├── git_panel.cpp      # Git integration
│   │   ├── process_panel.cpp  # Process list
│   │   ├── task_panel.cpp     # Task manager
│   │   └── log_panel.cpp      # Log viewer
│   └── utils/
│       └── config.cpp/h   # Settings
├── include/
│   └── nlohmann/         # JSON library
├── CMakeLists.txt        # Build configuration
├── build.bat             # Build script
├── setup.iss             # Installer config
├── devpulse.toml         # Settings file
└── README.md             # This file
```

---

## Tech Stack

| Component | Technology |
|-----------|------------|
| **Language** | C++17 |
| **UI Framework** | FTXUI (terminal UI) |
| **JSON** | nlohmann/json |
| **Build** | CMake + FetchContent |

---

© 2026 [Zenith Open Source Projects](https://zenithopensourceprojects.vercel.app/)
Made with ❤️ by @roshhellwett
