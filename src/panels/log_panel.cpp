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

#include "log_panel.h"
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <algorithm>

using namespace ftxui;

static std::string get_log_path() {
    wchar_t exe_path[MAX_PATH];
    if (GetModuleFileNameW(nullptr, exe_path, MAX_PATH) == 0) {
        return "devpulse.log";
    }
    std::wstring ws(exe_path);
    size_t pos = ws.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        ws = ws.substr(0, pos);
    }
    ws += L"\\devpulse.log";
    
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), nullptr, 0, nullptr, nullptr);
    if (size_needed == 0) return "devpulse.log";
    std::string result(size_needed, '\0');
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), (int)ws.size(), &result[0], size_needed, nullptr, nullptr);
    return result;
}

LogPanel::LogPanel(const std::string& path) 
    : log_path(get_log_path()), last_pos(0) {
    
    if (!log_path.empty()) {
        {
            std::ofstream out_file(log_path, std::ios::app);
            out_file << "[INFO] DevPulse started" << std::endl;
        }
        
        log_file.open(log_path, std::ios::in);
        if (log_file.is_open()) {
            read_all_lines();
        }
    }
}

void LogPanel::read_all_lines() {
    if (!log_file.is_open()) return;
    
    log_file.clear();
    log_file.seekg(0, std::ios::beg);
    
    lines.clear();
    std::string line;
    while (std::getline(log_file, line)) {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
            line.pop_back();
        }
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    
    log_file.clear();
    log_file.seekg(0, std::ios::end);
    last_pos = log_file.tellg();
}

void LogPanel::refresh() {
    if (!log_file.is_open()) {
        log_file.open(log_path, std::ios::in);
        if (log_file.is_open()) {
            read_all_lines();
        }
        return;
    }
    
    log_file.seekg(last_pos);
    std::string line;
    while (std::getline(log_file, line)) {
        while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) {
            line.pop_back();
        }
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    last_pos = log_file.tellg();

    if (log_file.eof()) {
        log_file.clear();
        log_file.seekg(last_pos);
    }
    
    while (lines.size() > MAX_LINES) {
        lines.erase(lines.begin());
    }
}

void LogPanel::set_log_path(const std::string& path) {
    log_path = path;
    last_pos = 0;
    lines.clear();
    log_file.close();
    log_file.open(log_path, std::ios::in);
    if (log_file.is_open()) {
        read_all_lines();
    }
}

Element LogPanel::render() {
    Elements els;
    
    els.push_back(text("LOG VIEWER") | bold | color(Color::Blue));
    
    if (!log_file.is_open()) {
        els.push_back(text("File not open") | color(Color::Yellow));
    } else if (lines.empty()) {
        els.push_back(text("No log entries") | color(Color::GrayDark));
    } else {
        for (auto& line : lines) {
            els.push_back(text(line) | color(Color::Cyan));
        }
    }
    
    return vbox(els) | border;
}
