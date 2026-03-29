/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#pragma once
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>
#include <windows.h>

struct ProcessInfo {
    int pid;
    std::string name;
    double cpu_percent = 0.0;
    size_t memory_mb = 0;
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
    ULARGE_INTEGER last_idle_time;
    ULARGE_INTEGER last_kernel_time;
    ULARGE_INTEGER last_user_time;
    bool first_run = true;
};
