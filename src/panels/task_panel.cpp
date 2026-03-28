/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "task_panel.h"
#include <ftxui/dom/elements.hpp>
#include <nlohmann/json.hpp>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <algorithm>

using json = nlohmann::json;
using namespace ftxui;

namespace fs = std::filesystem;

static std::string get_current_timestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M");
    return oss.str();
}

static std::string get_project_name() {
    std::error_code ec;
    fs::path cwd = fs::current_path(ec);
    if (ec) return "default";
    std::string path = cwd.string();
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos) {
        return path.substr(pos + 1);
    }
    return cwd.filename().string();
}

TaskPanel::TaskPanel() : selected_index(0), next_id(1) {
    if (!fs::exists("tasks")) {
        fs::create_directories("tasks");
    }
    task_file = "tasks/" + get_project_name() + ".json";
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
    Elements task_lines;
    
    int total = (int)tasks.size();
    int completed = 0;
    for (auto& t : tasks) {
        if (t.done) completed++;
    }
    
    int bar_width = 10;
    int filled = total > 0 ? (int)((float)completed / total * bar_width) : 0;
    std::string progress_bar = "[" + std::string(filled, '#') + std::string(bar_width - filled, '-') + "]";
    
    std::string progress_text = std::to_string(completed) + "/" + std::to_string(total) + " done";
    
    if (tasks.empty()) {
        task_lines.push_back(text("  No tasks yet") | color(Color::GrayDark));
        task_lines.push_back(text("  Press [a] to add") | color(Color::Cyan));
    } else {
        for (size_t i = 0; i < tasks.size(); ++i) {
            auto& t = tasks[i];
            bool is_selected = (int)i == selected_index;
            
            std::string checkbox = t.done ? "[x]" : "[ ]";
            Color check_color = t.done ? Color::Green : Color::White;
            Color text_color = t.done ? Color::GrayDark : Color::White;
            
            std::string name = t.text;
            if (name.length() > 25) {
                name = name.substr(0, 22) + "...";
            }
            
            Element line = hbox(Elements{
                text("  ") | flex,
                text(checkbox) | color(check_color) | bold,
                text("  ") | flex,
                text(name) | color(text_color),
                text("  ") | flex,
                text("@" + t.created_at) | color(Color::GrayDark),
            });
            
            if (is_selected) {
                line = line | inverted;
            }
            
            task_lines.push_back(line);
        }
    }

    Color progress_color = (completed == total && total > 0) ? Color::Green : Color::Yellow;

    Elements els;
    els.push_back(hbox(Elements{
        text(" TASK MANAGER ") | bold | color(Color::Green) | inverted,
    }));
    els.push_back(separator());
    els.push_back(hbox(Elements{
        text("  Progress: ") | color(Color::White),
        text(progress_bar) | color(progress_color),
        text("  ") | flex,
        text(progress_text) | bold | color(Color::Cyan),
    }));
    els.push_back(separator());
    els.push_back(vbox(task_lines) | flex);
    els.push_back(separator());
    els.push_back(text(""));
    return vbox(els) | border;
}
