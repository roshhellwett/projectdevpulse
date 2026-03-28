/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "process_panel.h"
#include <ftxui/dom/elements.hpp>
#include <dirent.h>
#include <cstring>
#include <algorithm>

using namespace ftxui;

static const char* DEV_PROCESSES[] = {
    "gcc", "g++", "clang", "clang++", "python", "python3", "node", "npm",
    "yarn", "vim", "nvim", "emacs", "make", "cmake", "cargo", "rustc",
    "go", "javac", "java", "perl", "ruby", "php", "lua", "dotnet", "gradle",
    "webpack", "vite", "next", "nuxt", "django", "flask", "rails", "spring",
    "mvn", "sbt", "zig", "swift", "kotlin", "scala"
};

static const size_t NUM_DEV_PROCESSES = sizeof(DEV_PROCESSES) / sizeof(DEV_PROCESSES[0]);

bool ProcessPanel::is_dev_process(const std::string& name) {
    std::string lower_name = name;
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
    for (size_t i = 0; i < NUM_DEV_PROCESSES; ++i) {
        if (lower_name == DEV_PROCESSES[i])
            return true;
    }
    return false;
}

std::vector<ProcessInfo> ProcessPanel::get_dev_processes() {
    std::vector<ProcessInfo> result;
    DIR* proc_dir = opendir("/proc");
    if (!proc_dir) return result;

    struct dirent* entry;
    while ((entry = readdir(proc_dir)) != nullptr) {
        if (entry->d_type != DT_DIR) continue;
        if (!isdigit(entry->d_name[0])) continue;

        int pid = atoi(entry->d_name);
        std::string comm_path = "/proc/" + std::string(entry->d_name) + "/comm";
        FILE* f = fopen(comm_path.c_str(), "r");
        if (!f) continue;

        char name[256] = {0};
        if (fgets(name, sizeof(name), f)) {
            name[strcspn(name, "\n")] = 0;
            if (is_dev_process(name)) {
                ProcessInfo info;
                info.pid = pid;
                info.name = name;
                result.push_back(info);
            }
        }
        fclose(f);
    }
    closedir(proc_dir);
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
