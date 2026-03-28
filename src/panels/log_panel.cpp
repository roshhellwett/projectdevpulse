/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "log_panel.h"
#include <ftxui/dom/elements.hpp>

using namespace ftxui;

LogPanel::LogPanel(const std::string& path) 
    : log_path(path), last_pos(0) {
    log_file.open(log_path, std::ios::in);
    if (log_file.is_open()) {
        log_file.seekg(0, std::ios::end);
        last_pos = log_file.tellg();
    }
    refresh();
}

void LogPanel::set_log_path(const std::string& path) {
    log_path = path;
    last_pos = 0;
    lines.clear();
    log_file.close();
    log_file.open(log_path, std::ios::in);
    if (log_file.is_open()) {
        log_file.seekg(0, std::ios::end);
        last_pos = log_file.tellg();
    }
}

void LogPanel::refresh() {
    if (!log_file.is_open()) {
        if (log_path.empty()) {
            log_path = "devpulse.log";
            log_file.open(log_path, std::ios::in);
        }
        if (!log_file.is_open()) {
            log_path = "output.log";
            log_file.open(log_path, std::ios::in);
        }
        if (log_file.is_open()) {
            log_file.seekg(0, std::ios::end);
            last_pos = log_file.tellg();
        }
        return;
    }

    log_file.seekg(last_pos);
    std::string line;
    while (std::getline(log_file, line)) {
        if (line.empty()) continue;
        lines.push_back(line);
        if (lines.size() > MAX_LINES) {
            lines.erase(lines.begin());
        }
    }
    last_pos = log_file.tellg();

    if (log_file.eof()) {
        log_file.clear();
        log_file.seekg(last_pos);
    }
}

Element LogPanel::render() {
    Elements log_lines;
    
    if (!log_file.is_open()) {
        log_lines.push_back(text("Log file not found") | color(Color::Red));
        log_lines.push_back(text("Create: devpulse.log") | color(Color::Cyan));
        log_lines.push_back(text("       output.log") | color(Color::Cyan));
    } else {
        for (auto& line : lines) {
            if (line.find("ERROR") != std::string::npos ||
                line.find("error") != std::string::npos ||
                line.find("FAIL") != std::string::npos) {
                log_lines.push_back(text(line) | color(Color::Red));
            } else if (line.find("WARN") != std::string::npos ||
                       line.find("warn") != std::string::npos) {
                log_lines.push_back(text(line) | color(Color::Yellow));
            } else if (line.find("INFO") != std::string::npos ||
                       line.find("info") != std::string::npos) {
                log_lines.push_back(text(line) | color(Color::Green));
            } else {
                log_lines.push_back(text(line) | color(Color::White));
            }
        }
        
        if (lines.empty()) {
            log_lines.push_back(text("Waiting for logs...") | color(Color::Cyan));
        }
    }

    return vbox({
        text("LOG VIEWER") | bold | color(Color::Blue),
        separator(),
        vbox(log_lines) | flex,
    }) | border;
}
