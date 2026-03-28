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

#include "panels/system_panel.h"
#include "panels/git_panel.h"
#include "panels/process_panel.h"
#include "panels/log_panel.h"
#include "panels/task_panel.h"
#include "utils/config.h"

class App {
public:
    App();
    ~App();
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
    
    void refresh_loop();
};
