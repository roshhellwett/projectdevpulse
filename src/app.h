/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#pragma once
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/dom/elements.hpp>
#include <atomic>
#include <string>

#include "panels/system_panel.h"
#include "panels/git_panel.h"
#include "panels/process_panel.h"
#include "panels/log_panel.h"
#include "panels/task_panel.h"
#include "utils/config.h"

class DevPulseApp {
public:
    DevPulseApp();
    ~DevPulseApp();
    void run();
private:
    ftxui::ScreenInteractive screen;
    SystemPanel sys_panel;
    GitPanel git_panel;
    ProcessPanel process_panel;
    LogPanel log_panel;
    TaskPanel task_panel;
    Config config;
    std::atomic<bool> running;
    bool input_mode;
    std::string input_buffer;
    int pulse_state;
    
    void refresh_loop();
    std::string get_pulse_indicator();
    std::string get_timestamp();
};
