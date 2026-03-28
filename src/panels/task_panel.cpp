/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "task_panel.h"
#include <ftxui/dom/elements.hpp>
#include <nlohmann/json.hpp>
#include <unistd.h>
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
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

static std::string get_project_name() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == nullptr) return "default";
    std::string path(cwd);
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos) {
        return path.substr(pos + 1);
    }
    return "default";
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
    // Nothing to refresh - tasks are event-driven
}

Element TaskPanel::render() {
    Elements task_lines;
    
    if (tasks.empty()) {
        task_lines.push_back(
            text("No tasks. Press 'a' to add.") | dim | color(Color::RGB(128, 128, 128))
        );
    } else {
        for (size_t i = 0; i < tasks.size(); ++i) {
            auto& t = tasks[i];
            bool is_selected = (int)i == selected_index;
            
            std::string checkbox = t.done ? "[x]" : "[ ]";
            Color check_color = t.done ? Color::Green : Color::White;
            Color text_color = t.done ? Color::RGB(128, 128, 128) : Color::White;
            
            Elements line_elements = {
                text(checkbox) | color(check_color),
                text(" ") | flex,
                text(t.text) | color(text_color),
            };
            auto line = hbox(line_elements);
            
            if (is_selected) {
                line = line | inverted;
            }
            
            task_lines.push_back(line);
        }
    }

    return vbox({
        text("TASKS") | bold | color(Color::Green),
        separator(),
        vbox(task_lines) | flex,
        separator(),
        text("a:add  d:toggle  x:delete") | dim,
    }) | border;
}
