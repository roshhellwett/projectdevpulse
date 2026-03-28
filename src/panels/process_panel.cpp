/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "process_panel.h"
#include <ftxui/dom/elements.hpp>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <cstring>
#include <algorithm>
#include <vector>

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "psapi.lib")

using namespace ftxui;

static const char* DEV_PROCESSES[] = {
    "gcc", "g++", "clang", "clang++", "python", "python3", "python2", "node", "npm",
    "yarn", "vim", "nvim", "emacs", "make", "cmake", "cargo", "rustc",
    "go", "javac", "java", "perl", "ruby", "php", "lua", "dotnet", "gradle",
    "webpack", "vite", "next", "nuxt", "django", "flask", "rails", "spring",
    "mvn", "sbt", "zig", "swift", "kotlin", "scala",
    "code", "devenv", "clion", "pycharm", "webstorm", "goland",
    "sublime_text", "notepad++", "git", "cmd", "powershell", "terminal"
};

static const size_t NUM_DEV_PROCESSES = sizeof(DEV_PROCESSES) / sizeof(DEV_PROCESSES[0]);

bool ProcessPanel::is_dev_process(const std::string& name) {
    std::string lower_name = name;
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
    for (size_t i = 0; i < NUM_DEV_PROCESSES; ++i) {
        if (lower_name.find(DEV_PROCESSES[i]) != std::string::npos)
            return true;
    }
    return false;
}

std::vector<ProcessInfo> ProcessPanel::get_dev_processes() {
    std::vector<ProcessInfo> result;
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return result;
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(snapshot, &pe32)) {
        do {
            std::string process_name = pe32.szExeFile;
            if (is_dev_process(process_name)) {
                ProcessInfo info;
                info.pid = pe32.th32ProcessID;
                info.name = process_name;
                result.push_back(info);
            }
        } while (Process32Next(snapshot, &pe32));
    }
    
    CloseHandle(snapshot);
    return result;
}

ProcessPanel::ProcessPanel() { refresh(); }

void ProcessPanel::refresh() {
    processes = get_dev_processes();
}

Element ProcessPanel::render() {
    Elements lines;

    if (processes.empty()) {
        lines.push_back(text("No dev processes running") | color(Color::Cyan));
    } else {
        for (auto& p : processes) {
            lines.push_back(hbox({
                text(std::to_string(p.pid)) | color(Color::Cyan) | flex,
                text(" ") | flex,
                text(p.name) | color(Color::Green) | flex,
            }));
        }
    }

    return vbox({
        text("PROCESSES") | bold | color(Color::Magenta),
        separator(),
        vbox(lines) | flex,
    }) | border;
}
