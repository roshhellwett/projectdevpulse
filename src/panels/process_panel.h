/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#pragma once
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

struct ProcessInfo {
    int pid;
    std::string name;
};

class ProcessPanel {
public:
    ProcessPanel();
    void refresh();
    ftxui::Element render();
private:
    std::vector<ProcessInfo> processes;
    std::vector<ProcessInfo> get_dev_processes();
    bool is_dev_process(const std::string& name);
};
