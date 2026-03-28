/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "git_panel.h"
#include <ftxui/dom/elements.hpp>
#include <nlohmann/json.hpp>
#include <array>
#include <memory>
#include <stdexcept>
#include <cstdio>

using namespace ftxui;
using json = nlohmann::json;

GitPanel::GitPanel() { refresh(); }

std::string GitPanel::run_python_script() {
    std::string cmd = "python3 src/core/gitreader.py . 2>/dev/null";
    std::array<char, 512> buf;
    std::string output;
    std::unique_ptr<FILE, decltype(&pclose)>
        pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "{}";
    while (fgets(buf.data(), buf.size(), pipe.get()))
        output += buf.data();
    return output;
}

GitInfo GitPanel::parse_output(const std::string& json_str) {
    GitInfo g;
    try {
        auto j = json::parse(json_str);
        g.is_git = j.value("is_git", false);
        g.branch = j.value("branch", "");
        for (auto& s : j.value("status",  json::array()))
            g.status.push_back(s.get<std::string>());
        for (auto& c : j.value("commits", json::array()))
            g.commits.push_back(c.get<std::string>());
    } catch (...) { g.is_git = false; }
    return g;
}

void GitPanel::refresh() {
    info = parse_output(run_python_script());
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
