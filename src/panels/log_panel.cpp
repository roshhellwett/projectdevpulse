/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "log_panel.h"
#include <ftxui/dom/elements.hpp>
#include <ctime>
#include <iomanip>

using namespace ftxui;

static std::string get_timestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
    return std::string(buf);
}

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
        log_lines.push_back(text("  No log file found") | color(Color::Yellow));
        log_lines.push_back(text("  Create: devpulse.log") | color(Color::GrayDark));
        log_lines.push_back(text("       output.log") | color(Color::GrayDark));
        log_lines.push_back(text("  or: ./devpulse > log.txt") | color(Color::Cyan));
    } else {
        for (auto& line : lines) {
            std::string prefix = "  " + get_timestamp() + " | ";
            
            if (line.find("ERROR") != std::string::npos ||
                line.find("error") != std::string::npos ||
                line.find("FAIL") != std::string::npos ||
                line.find("FATAL") != std::string::npos) {
                log_lines.push_back(hbox(Elements{
                    text(prefix) | color(Color::GrayDark),
                    text("[ERR]") | bold | color(Color::Red),
                    text(" ") | flex,
                    text(line) | color(Color::Red),
                }));
            } else if (line.find("WARN") != std::string::npos ||
                       line.find("warn") != std::string::npos ||
                       line.find("WARNING") != std::string::npos) {
                log_lines.push_back(hbox(Elements{
                    text(prefix) | color(Color::GrayDark),
                    text("[WRN]") | bold | color(Color::Yellow),
                    text(" ") | flex,
                    text(line) | color(Color::Yellow),
                }));
            } else if (line.find("INFO") != std::string::npos ||
                       line.find("info") != std::string::npos ||
                       line.find("DEBUG") != std::string::npos) {
                log_lines.push_back(hbox(Elements{
                    text(prefix) | color(Color::GrayDark),
                    text("[INF]") | bold | color(Color::Cyan),
                    text(" ") | flex,
                    text(line) | color(Color::White),
                }));
            } else if (line.find("SUCCESS") != std::string::npos ||
                       line.find("OK") != std::string::npos) {
                log_lines.push_back(hbox(Elements{
                    text(prefix) | color(Color::GrayDark),
                    text("[OK ]") | bold | color(Color::Green),
                    text(" ") | flex,
                    text(line) | color(Color::Green),
                }));
            } else {
                log_lines.push_back(hbox(Elements{
                    text(prefix) | color(Color::GrayDark),
                    text("[LOG]") | color(Color::GrayDark),
                    text(" ") | flex,
                    text(line) | color(Color::White),
                }));
            }
        }
        
        if (lines.empty()) {
            log_lines.push_back(text("  Waiting for log entries...") | color(Color::GrayDark));
        }
    }

    Elements els;
    els.push_back(hbox(Elements{
        text(" LOG VIEWER ") | bold | color(Color::Blue) | inverted,
    }));
    els.push_back(separator());
    els.push_back(vbox(log_lines) | flex);
    els.push_back(separator());
    els.push_back(text(""));
    return vbox(els) | border;
}
