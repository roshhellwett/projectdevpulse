# devpulse

> A TUI Developer Workspace Dashboard for Linux

Built and maintained by **Zenith Open Source Projects**

## Features

- **System Stats**: Real-time CPU, RAM, and disk usage via /proc
- **Git Integration**: Branch, status, and recent commits via Python subprocess
- **Process Monitor**: Filter and track development processes
- **Log Viewer**: Live tail system logs with color-coded entries
- **Task Tracker**: Per-project persistent TODO list stored as JSON

## Install

```bash
git clone https://github.com/zenith-open-source/devpulse
cd devpulse
./scripts/install_deps.sh
./build.sh
```

## Run

```bash
./run.sh
```

## Keyboard Shortcuts

| Key | Action |
|-----|--------|
| q | Quit |
| a | Add task (in task panel) |
| d | Toggle task done |
| x | Delete task |
| Arrow Up/Down | Navigate tasks |

## Configuration

Edit `devpulse.toml` to customize:

- Refresh rate
- Panel visibility
- Log file path
- Process filters

## Tech Stack

- C++17 with FTXUI for TUI rendering
- C for low-level /proc system calls
- Python 3 for Git integration
- nlohmann/json for task persistence

## License

MIT — Zenith Open Source Projects 2026
