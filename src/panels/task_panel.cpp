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
#include <shlobj.h>

#include "task_panel.h"
#include <ftxui/dom/elements.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <algorithm>

using json = nlohmann::json;
using namespace ftxui;

static std::string get_current_timestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M");
    return oss.str();
}

static bool should_exclude_from_tasks(const std::string& text) {
    static const char* blacklist[] = {
        "tasks.json", "task.json", "devpulse.log", "output.log", "log.txt",
        "devpulse.exe", "devpulse-data", "package.json", "CMakeLists.txt",
        ".git", ".github", "README.md", "LICENSE", "Makefile", ".gitignore"
    };
    std::string lower = text;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    for (const auto& excluded : blacklist) {
        if (lower == excluded) return true;
        if (lower.find(excluded) != std::string::npos) return true;
    }
    return false;
}

static bool create_directory(const std::string& path) {
    return CreateDirectoryA(path.c_str(), NULL) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
}

static std::string get_exe_dir_fallback() {
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
    std::string result(size_needed - 1, char(0));
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, &result[0], size_needed, nullptr, nullptr);
    return result;
}

static std::string get_appdata_dir() {
    wchar_t appdata_path[MAX_PATH];
    if (SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, appdata_path) != S_OK) {
        return get_exe_dir_fallback();
    }
    std::wstring ws(appdata_path);
    ws += L"\\DevPulse";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, nullptr, 0, nullptr, nullptr);
    if (size_needed == 0) return ".";
    std::string result(size_needed - 1, char(0));
    WideCharToMultiByte(CP_UTF8, 0, ws.c_str(), -1, &result[0], size_needed, nullptr, nullptr);
    return result;
}

TaskPanel::TaskPanel() : selected_index(0), next_id(1) {
    std::string data_dir = get_appdata_dir();
    create_directory(data_dir);
    task_file = data_dir + "\\tasks.json";
    load_tasks();
}

TaskPanel::~TaskPanel() {
    save_tasks();
}

void TaskPanel::load_tasks() {
    std::ifstream f(task_file);
    if (!f.is_open()) return;
    
    try {
        json j;
        f >> j;
        for (auto& item : j) {
            Task t;
            t.id = item.value("id", 0);
            t.text = item.value("text", "");
            t.done = item.value("done", false);
            t.created_at = item.value("created_at", "");
            tasks.push_back(t);
            if (t.id >= next_id) next_id = t.id + 1;
        }
        tasks.erase(
            std::remove_if(tasks.begin(), tasks.end(),
                [](const Task& t) {
                    return t.text == "task.json" || 
                           (t.text.length() >= 5 && t.text.substr(t.text.length() - 5) == ".json");
                }),
            tasks.end()
        );
    } catch (...) {}
}

void TaskPanel::save_tasks() {
    json j = json::array();
    for (auto& t : tasks) {
        j.push_back({
            {"id", t.id},
            {"text", t.text},
            {"done", t.done},
            {"created_at", t.created_at}
        });
    }
    std::ofstream f(task_file);
    if (f.is_open()) {
        f << j.dump(2);
    }
}

void TaskPanel::add_task(const std::string& text) {
    if (should_exclude_from_tasks(text)) return;
    Task t;
    t.id = next_id++;
    t.text = text;
    t.done = false;
    t.created_at = get_current_timestamp();
    tasks.push_back(t);
    save_tasks();
}

void TaskPanel::toggle_task(int id) {
    for (auto& t : tasks) {
        if (t.id == id) {
            t.done = !t.done;
            break;
        }
    }
    save_tasks();
}

void TaskPanel::delete_task(int id) {
    tasks.erase(
        std::remove_if(tasks.begin(), tasks.end(), 
            [id](const Task& t) { return t.id == id; }),
        tasks.end()
    );
    if (selected_index >= (int)tasks.size() && selected_index > 0) {
        selected_index = tasks.size() - 1;
    }
    save_tasks();
}

void TaskPanel::set_selected(int index) {
    if (index >= 0 && index < (int)tasks.size()) {
        selected_index = index;
    }
}

void TaskPanel::refresh() {
}

Element TaskPanel::render() {
    Elements els;
    
    int total = (int)tasks.size();
    int completed = 0;
    for (auto& t : tasks) {
        if (t.done) completed++;
    }
    
    std::string status = (completed == total && total > 0) ? "All done!" : "In progress";
    els.push_back(text("TASKS: " + std::to_string(completed) + "/" + std::to_string(total) + " - " + status) | bold | color(Color::Green));
    
    if (tasks.empty()) {
        els.push_back(text("No tasks - press [a] to add") | color(Color::GrayDark));
    } else {
        for (size_t i = 0; i < tasks.size(); ++i) {
            auto& t = tasks[i];
            bool is_selected = (int)i == selected_index;
            
            std::string checkbox = t.done ? "[x]" : "[ ]";
            std::string display = checkbox + " " + t.text + " @" + t.created_at;
            
            Color text_color = t.done ? Color::GrayDark : (is_selected ? Color::Cyan : Color::White);
            els.push_back(text(display) | color(text_color));
        }
    }
    
    return vbox(els) | border;
}
