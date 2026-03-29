<h1 align=>
  PROJECT DEV PULSE
</h1>
<p align=>
  <img src="https://img.shields.io/badge/Version-1.0.0-00d9ff?style=flat-square" alt="Version">
  <img src="https://img.shields.io/badge/License-MIT-ff6b6b?style=flat-square" alt="License">
  <img src="https://img.shields.io/badge/Platform-Windows%20x64-00ff88?style=flat-square" alt="Platform">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue?style=flat-square" alt="C++">
  <img src="https://img.shields.io/badge/UI%20Framework-FTXUI-9900ff?style=flat-square" alt="FTXUI">
</p>

---

<p align=>
DevPulse is a sleek, real-time terminal dashboard that keeps your development workspace visible at a glance. No more context switching between terminals — system stats, git status, dev processes, logs, and tasks all in one place.
</p>

---

### System Monitor
- CPU usage with animated progress bar
- RAM utilization tracking
- Color-coded thresholds (green → yellow → red)

### Git Integration
- Current branch display with badge
- File status indicators (modified, added, deleted)
- Recent commits at a glance

### Process Tracker
- Auto-detects developer tools (gcc, g++, clang, make, cmake, node, etc.)
- Process count with live updates
- Memory usage per process

### Task Manager
- Quick-add tasks with keyboard shortcuts
- Mark complete/incomplete with visual feedback
- Persistent storage via JSON

### Log Viewer
- Real-time log tailing
- Color-coded severity levels [ERR] [WRN] [INF] [OK]
- Timestamped entries

---

### Prerequisites
- Windows 10/11 x64
- [MSYS2](https://www.msys2.org/) with MinGW-w64 (gcc, cmake)

### Build & Run

```powershell
# Clone the repository
git clone https://github.com/zenith-opensource/devpulse
cd devpulse

# Run the build script
.\build.bat

# Launch
.\dist\devpulse.exe
```

### Installer (Optional)

For a proper Windows installation, use Inno Setup:

```powershell
iscc setup.iss
```

This creates `DevPulse-Setup-1.0.0.exe` with Start Menu shortcuts.

---

## Keyboard Controls

| Key | Action |
|-----|--------|
| `q` | Quit application |
| `a` | Add new task |
| `d` | Toggle task done/undone |
| `x` | Delete selected task |
| `↑` / `↓` | Navigate task list |

---

## Configuration

Edit `devpulse.toml` to customize behavior:

```toml
[general]
refresh_rate_ms = 1000
theme = "dark"

[panels]
show_system  = true
show_git     = true
show_process = true
show_log     = true
show_tasks   = true

[log]
file = "devpulse.log"

[process]
filter = ["gcc", "g++", "clang", "clang++", "python", "node", "npm", "vim", "nvim", "make", "cmake", "clangd"]
```

---

## Architecture

```
devpulse/
├── src/
│   ├── app.cpp/h          # Main application & UI rendering
│   ├── main.cpp          # Entry point
│   ├── core/
│   │   └── sysreader_win.cpp  # Windows system stats API
│   ├── panels/
│   │   ├── system_panel.cpp   # CPU/RAM display
│   │   ├── git_panel.cpp      # Git status integration
│   │   ├── process_panel.cpp  # Process enumeration
│   │   ├── task_panel.cpp     # Task management
│   │   └── log_panel.cpp      # Log viewer
│   └── utils/
│       └── config.cpp/h   # Configuration loader
├── include/
│   └── nlohmann/         # JSON library (header-only)
├── CMakeLists.txt        # Build configuration
├── build.bat             # Windows build script
├── setup.iss             # Inno Setup installer
└── devpulse.toml         # Runtime configuration
```

---

## Tech Stack

| Component | Technology |
|-----------|-----------|
| **Language** | C++17 |
| **UI Framework** | FTXUI (terminal UI) |
| **JSON** | nlohmann/json (header-only) |
| **Build** | CMake + FetchContent |
| **Installer** | Inno Setup |

---

© 2026 [Zenith Open Source Projects](https://zenithopensourceprojects.vercel.app/). All Rights Reserved. 
Zenith is a Open Source Project Idea's by @roshhellwett
