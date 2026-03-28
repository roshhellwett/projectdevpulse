# devpulse

**Developer Workspace Dashboard — Built for Linux**

A single terminal window that keeps everything you need while coding in view. No switching between tabs. No context switching. Just your system, your code, and your tasks — all visible at a glance.

---

## What It Does

```
┌─────────────────────────────────────────────────────────────┐
│                     SYSTEM MONITORING                        │
│                                                             │
│   CPU:    [████████░░░░░░░░░░░] 40%                       │
│   RAM:    [███████░░░░░░░░░░░░░] 35%                       │
│   DISK:   [████████████░░░░░░░░] 62%                       │
│                                                             │
│   PROCESSES (dev tools running)                             │
│   gcc, make, python3...                                     │
│                                                             │
├─────────────────────────────────────────────────────────────┤
│                        GIT STATUS                           │
│                                                             │
│   Branch: main                                             │
│   Status: M src/app.cpp  A README.md                       │
│   Commits: latest commit message here                       │
│                                                             │
│   LOG VIEWER                                               │
│   [color-coded system logs in real-time]                    │
│                                                             │
│   TASK TRACKER (per-project TODO)                           │
│   [x] Fix authentication bug                                │
│   [ ] Add dark mode                                         │
│   [ ] Write unit tests                                      │
└─────────────────────────────────────────────────────────────┘
```

---

## Who Is It For?

| You have... | devpulse helps by... |
|-------------|---------------------|
| Multiple terminals open | One window shows everything |
| Switching between tools to check status | System stats always visible |
| Forgetting to track tasks | Built-in todo list per project |
| Missing important log errors | Color-coded log viewer |
| Lost git context | Current branch/status always shown |

---

## Quick Start

```bash
git clone https://github.com/zenith-open-source/devpulse
cd devpulse
./build.sh    # Installs dependencies + builds (first time only)
./run.sh      # Run devpulse
```

---

## Controls

| Key | Action |
|-----|--------|
| `a` | Add new task |
| `d` | Toggle task done/undone |
| `x` | Delete selected task |
| `↑` `↓` | Navigate task list |
| `q` | Quit |

---

## Configuration

Edit `devpulse.toml` to customize:

```toml
refresh_rate_ms = 1000    # How often to update (milliseconds)
log_file = "/var/log/syslog"  # Log file to watch
```

---

## Requirements

- Linux (Ubuntu, Debian, Arch)
- GCC/G++ compiler
- CMake
- Python 3 + gitpython
- 1920x1080 or larger terminal

---

## Tech Stack

| Layer | Technology |
|-------|------------|
| UI Framework | FTXUI (C++ terminal UI) |
| System Calls | C via /proc filesystem |
| Git Integration | Python subprocess |
| Data Storage | nlohmann/json |

---

## License

MIT — Zenith Open Source Projects
