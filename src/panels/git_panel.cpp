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
    if (branch.empty()) {
        branch = run_command("git symbolic-ref --short HEAD");
        if (!branch.empty() && branch.back() == '\n') branch.pop_back();
    }
    g.branch = branch;
    
    std::string status = run_command("git status --porcelain");
    if (!status.empty()) {
        std::istringstream iss(status);
        std::string line;
        while (std::getline(iss, line)) {
            if (line.length() >= 3) {
                g.status.push_back(line.substr(0, 2) + " " + line.substr(3));
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
        Elements els;
        els.push_back(hbox(Elements{
            text(" GIT REPOSITORY ") | bold | color(Color::Yellow) | inverted,
        }));
        els.push_back(separator());
        els.push_back(text("") | flex);
        els.push_back(text("  Not a git repository") | color(Color::Red));
        els.push_back(text("  Run: git init") | color(Color::GrayDark));
        els.push_back(text("") | flex);
        els.push_back(separator());
        els.push_back(text(""));
        return vbox(els) | border;
    }

    bool is_clean = info.status.empty();
    std::string status_text = is_clean ? "Clean" : std::to_string(info.status.size()) + " change(s)";
    Color status_color = is_clean ? Color::Green : Color::Red;
    
    Elements status_lines;
    if (!is_clean) {
        for (auto& s : info.status) {
            std::string indicator = s.substr(0, 2);
            std::string file = s.length() > 2 ? s.substr(3) : "";
            
            Color ind_color = Color::White;
            if (indicator == " M" || indicator == "M ") ind_color = Color::Yellow;
            if (indicator == " A" || indicator == "A ") ind_color = Color::Green;
            if (indicator == " D" || indicator == "D ") ind_color = Color::Red;
            
            if (file.length() > 25) {
                file = file.substr(0, 22) + "...";
            }
            
            status_lines.push_back(hbox(Elements{
                text("  ") | flex,
                text(indicator) | bold | color(ind_color),
                text("  ") | flex,
                text(file) | color(Color::White),
            }));
        }
    }

    Elements commit_lines;
    for (auto& c : info.commits) {
        std::string hash = c.substr(0, 7);
        std::string msg = c.length() > 7 ? c.substr(8) : "";
        
        if (msg.length() > 22) {
            msg = msg.substr(0, 19) + "...";
        }
        
        commit_lines.push_back(hbox(Elements{
            text("  ") | flex,
            text(hash) | color(Color::Cyan),
            text(" ") | flex,
            text(msg) | color(Color::White),
        }));
    }

    Elements els;
    els.push_back(hbox(Elements{
        text(" GIT REPOSITORY ") | bold | color(Color::Yellow) | inverted,
    }));
    els.push_back(separator());
    els.push_back(hbox(Elements{
        text("  Branch: ") | bold | color(Color::White),
        text("[ ") | color(Color::GrayDark),
        text(info.branch) | bold | color(Color::Cyan),
        text(" ]") | color(Color::GrayDark),
    }));
    els.push_back(separator());
    els.push_back(hbox(Elements{
        text("  Status: ") | bold | color(Color::White),
        text(status_text) | bold | color(status_color),
    }));
    els.push_back(vbox(status_lines) | flex);
    els.push_back(separator());
    els.push_back(text("  Recent Commits") | bold | color(Color::White));
    els.push_back(vbox(commit_lines) | flex);
    els.push_back(separator());
    els.push_back(text(""));
    return vbox(els) | border;
}
