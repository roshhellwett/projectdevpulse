<p align="center">
  <img src="https://raw.githubusercontent.com/zenith-opensource/devpulse/main/.assets/logo.png" alt="DevPulse" width="200" style="filter: drop-shadow(0 0 20px rgba(0,255,136,0.3));">
</p>

<h1 align="center">
  <code>Project Dev Pulse</code>
</h1>

<p align="center">
  <strong>Developer Dashboard for Windows</strong><br>
  Real-time system monitoring, git status, process tracking & task management
</p>

<p align="center">
  <img src="https://img.shields.io/badge/Version-1.0.0-00d9ff?style=flat-square" alt="Version">
  <img src="https://img.shields.io/badge/License-MIT-ff6b6b?style=flat-square" alt="License">
  <img src="https://img.shields.io/badge/Platform-Windows%20x64-00ff88?style=flat-square" alt="Platform">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue?style=flat-square" alt="C++">
  <img src="https://img.shields.io/badge/UI%20Framework-FTXUI-9900ff?style=flat-square" alt="FTXUI">
</p>

---

## What is DevPulse?

DevPulse is a sleek, real-time terminal dashboard that keeps your development workspace visible at a glance. No more context switching between terminals — system stats, git status, dev processes, logs, and tasks all in one place.

<p align="center">
  <img src="https://raw.githubusercontent.com/zenith-opensource/devpulse/main/.assets/screenshot.png" alt="DevPulse Screenshot" width="800">
</p>

---

## Features

### 🖥️ System Monitor
- CPU usage with animated progress bar
- RAM utilization tracking
- Color-coded thresholds (green → yellow → red)

### 📊 Git Integration
- Current branch display with badge
- File status indicators (modified, added, deleted)
- Recent commits at a glance

### ⚙️ Process Tracker
- Auto-detects developer tools (gcc, g++, clang, make, cmake, node, etc.)
- Process count with live updates
- Memory usage per process

### 📝 Task Manager
- Quick-add tasks with keyboard shortcuts
- Mark complete/incomplete with visual feedback
- Persistent storage via JSON

### 📜 Log Viewer
- Real-time log tailing
- Color-coded severity levels [ERR] [WRN] [INF] [OK]
- Timestamped entries

---

## Quick Start

### Prerequisites
- Windows 10/11 x64
- [MSYS2](https://www.msys2.org/) with MinGW-w64

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

## System Requirements

| Requirement | Minimum |
|------------|---------|
| OS | Windows 10/11 (x64) |
| RAM | 4 GB |
| Display | 1024x768 terminal |
| Dependencies | MSYS2 + MinGW-w64 |

---

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing`)
5. Open a Pull Request

---

## License

MIT License © 2026 **Zenith Open Source Projects**

Permission is hereby granted, free of charge, to any person obtaining a copy of this software.

---

<p align="center">
  <strong>Made with ❤️ by developers, for developers</strong><br>
  <sub>Project Dev Pulse v1.0.0</sub>
</p>

<p align="center">
  <a href="https://github.com/zenith-opensource">GitHub</a> •
  <a href="https://github.com/zenith-opensource/devpulse/releases">Releases</a>
</p>
