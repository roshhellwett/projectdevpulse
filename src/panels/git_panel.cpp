/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

#include "git_panel.h"
#include <ftxui/dom/elements.hpp>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>

using namespace ftxui;

static std::string run_command(const char* cmd) {
    std::string result;
    FILE* pipe = _popen(cmd, "r");
    if (!pipe) return "";
    char buf[512];
    while (fgets(buf, sizeof(buf), pipe)) {
        result += buf;
    }
    _pclose(pipe);
    return result;
}

static bool dir_exists(const char* path) {
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY));
}

static std::string get_exe_dir() {
    wchar_t exe_path[MAX_PATH];
    if (GetModuleFileNameW(nullptr, exe_path, MAX_PATH) == 0) {
        return ".";
    }
    std::wstring ws(exe_path);
    size_t pos = ws.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        ws = ws.substr(0, pos);
    }
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (size_needed == 0) return ".";
    std::string result(size_needed - 1, '\0');
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, &result[0], size_needed, nullptr, nullptr);
    return result;
}

static std::string find_git_root(std::string start_dir) {
    std::string dir = start_dir;
    while (!dir.empty()) {
        std::string git_dir = dir + "\\.git";
        if (dir_exists(git_dir.c_str())) {
            return dir;
        }
        size_t pos = dir.find_last_of("\\/");
        if (pos == std::string::npos) break;
        dir = dir.substr(0, pos);
    }
    return "";
}

GitPanel::GitPanel() { refresh(); }

void GitPanel::refresh() {
    GitInfo g;
    
    std::string exe_dir = get_exe_dir();
    std::string git_root = find_git_root(exe_dir);
    
    if (git_root.empty()) {
        info = g;
        return;
    }
    
    g.is_git = true;
    
    std::string cmd_branch = std::string("git -C \"") + git_root + "\" branch --show-current";
    std::string branch = run_command(cmd_branch.c_str());
    if (!branch.empty() && branch.back() == '\n') branch.pop_back();
    if (branch.empty()) {
        std::string cmd_ref = std::string("git -C \"") + git_root + "\" symbolic-ref --short HEAD";
        branch = run_command(cmd_ref.c_str());
        if (!branch.empty() && branch.back() == '\n') branch.pop_back();
    }
    g.branch = branch;
    
    std::string cmd_status = std::string("git -C \"") + git_root + "\" status --porcelain";
    std::string status = run_command(cmd_status.c_str());
    if (!status.empty()) {
        std::istringstream iss(status);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.length() >= 3) {
                g.status.push_back(line.substr(0, 2) + " " + line.substr(3));
            }
        }
    }
    
    std::string cmd_log = std::string("git -C \"") + git_root + "\" log --oneline -5 --format=\"%h %s\"";
    std::string commits = run_command(cmd_log.c_str());
    if (!commits.empty()) {
        std::istringstream iss(commits);
        std::string line;
        while (std::getline(iss, line)) {
            g.commits.push_back(line);
        }
    }
    
    info = g;
}

Element GitPanel::render() {
    Elements els;
    
    if (!info.is_git) {
        els.push_back(text("GIT: Not a git repository") | color(Color::Red));
        els.push_back(text("Run: git init") | color(Color::GrayDark));
        return vbox(els) | border;
    }

    bool is_clean = info.status.empty();
    Color status_color = is_clean ? Color::Green : Color::Red;
    
    els.push_back(text("GIT: Branch [" + info.branch + "] Status: " + 
                      (is_clean ? "Clean" : std::to_string(info.status.size()) + " changes")) | color(Color::Yellow));
    
    if (!is_clean) {
        els.push_back(text("Changed Files:") | bold);
        for (auto& s : info.status) {
            std::string indicator = s.substr(0, 2);
            std::string file = s.length() > 2 ? s.substr(3) : "";
            els.push_back(text("  " + indicator + " " + file) | color(Color::White));
        }
    }
    
    els.push_back(text("Recent Commits:") | bold);
    for (auto& c : info.commits) {
        std::string hash = c.substr(0, 7);
        std::string msg = c.length() > 7 ? c.substr(8) : "";
        els.push_back(text("  " + hash + " " + msg) | color(Color::White));
    }
    
    return vbox(els) | border;
}
