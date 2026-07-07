# Projectdevpulse

> A command-line tool written in C++ — Projectdevpulse.

![Language](https://img.shields.io/badge/Language-C%2B%2B-00599C)  ![License](https://img.shields.io/badge/License-MIT-blue)

## Overview

DevPulse is a real-time system monitoring dashboard that runs in the terminal. It tracks system metrics, running processes, active network connections, and git repository status — all in one place. Built with C++ for performance and accuracy.

## Prerequisites

- C++ installed on your system
- Git

## Installation

```bash
git clone https://github.com/roshhellwett/projectdevpulse.git
cd projectdevpulse
cmake -B build && cmake --build build
```

## Usage

```bash
./build/projectname
```

## Project Structure

```
├── include
│   └── nlohmann
│       └── json.hpp
├── sample
│   └── sample.png
├── src
│   ├── core
│   │   ├── sysreader.h
│   │   └── sysreader_win.cpp
│   ├── panels
│   │   ├── git_panel.cpp
│   │   ├── git_panel.h
│   │   ├── log_panel.cpp
│   │   ├── log_panel.h
│   │   ├── process_panel.cpp
│   │   ├── process_panel.h
│   │   ├── system_panel.cpp
│   │   ├── system_panel.h
│   │   ├── task_panel.cpp
│   │   └── task_panel.h
│   ├── utils
│   │   ├── config.cpp
│   │   └── config.h
│   ├── app.cpp
│   ├── app.h
│   └── main.cpp
├── build.bat
├── CMakeLists.txt
├── CUSTOM_README.md
├── DevPulse-1.0.0-win64.zip
├── devpulse.manifest
├── devpulse.rc
├── devpulse.toml
├── license
├── readme.md
├── security.md
├── setup.iss
└── vcpkg.json
```

## Notes

- Uses the `nlohmann/json` header for JSON parsing
- Windows system calls via the Win32 API (`sysreader_win.cpp`)
- Configuration stored in `devpulse.toml`
- Portable Windows build available as `DevPulse-1.0.0-win64.zip`

---

© 2026 [Zenith Open Source Projects](https://zenithopensourceprojects.vercel.app/). All Rights Reserved.
Zenith is an Open Source Project Idea by @roshhellwett
