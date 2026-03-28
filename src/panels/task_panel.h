/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#pragma once
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

struct Task {
    int id;
    std::string text;
    bool done;
    std::string created_at;
};

class TaskPanel {
public:
    TaskPanel();
    ~TaskPanel();
    void refresh();
    ftxui::Element render();
    void add_task(const std::string& text);
    void toggle_task(int id);
    void delete_task(int id);
    void set_selected(int index);
    int get_selected() const { return selected_index; }
private:
    std::vector<Task> tasks;
    int selected_index;
    std::string task_file;
    void load_tasks();
    void save_tasks();
    int next_id;
};
