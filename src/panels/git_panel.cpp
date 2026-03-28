/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "git_panel.h"
#include <ftxui/dom/elements.hpp>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <sstream>

using namespace ftxui;
namespace fs = std::filesystem;

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

GitPanel::GitPanel() { refresh(); }

void GitPanel::refresh() {
    GitInfo g;
    
    fs::path cwd = fs::current_path();
    fs::path git_dir = cwd / ".git";
    
    if (!fs::exists(git_dir)) {
        info = g;
        return;
    }
    
    g.is_git = true;
    
    std::string branch = run_command("git branch --show-current");
    if (!branch.empty() && branch.back() == '\n') branch.pop_back();
    g.branch = branch;
    
    std::string status = run_command("git status --porcelain");
    if (!status.empty()) {
        std::istringstream iss(status);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.length() >= 3) {
                g.status.push_back(line.substr(3));
            }
        }
    }
    
    std::string commits = run_command("git log --oneline -5");
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
    if (!info.is_git) {
        return vbox({
            text("GIT") | bold | color(Color::Yellow),
            separator(),
            text("Not a git repository") | color(Color::Cyan),
        }) | border;
    }

    Elements status_lines;
    for (auto& s : info.status)
        status_lines.push_back(text(s) | color(Color::Red));
    if (info.status.empty())
        status_lines.push_back(text("Clean") | color(Color::Green));

    Elements commit_lines;
    for (auto& c : info.commits)
        commit_lines.push_back(text(c) | color(Color::White));

    return vbox({
        text("GIT") | bold | color(Color::Yellow),
        separator(),
        hbox({ text("Branch: ") | bold,
               text(info.branch) | color(Color::Green) }),
        text("Status:") | bold,
        vbox(status_lines),
        separator(),
        text("Recent Commits:") | bold,
        vbox(commit_lines),
    }) | border;
}
