/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "app.h"
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <thread>
#include <chrono>
#include <mutex>
#include <ctime>

using namespace ftxui;

static std::mutex refresh_mutex;

DevPulseApp::DevPulseApp() 
    : config(ConfigLoader().get_config()),
      running(true),
      input_mode(false),
      input_buffer(""),
      pulse_state(0) {
}

DevPulseApp::~DevPulseApp() {
    running = false;
    delete sys_panel;
    delete git_panel;
    delete process_panel;
    delete log_panel;
    delete task_panel;
    delete screen;
}

void DevPulseApp::init_all() {
    if (initialized) return;
    screen = new ftxui::ScreenInteractive(ScreenInteractive::Fullscreen());
    sys_panel = new SystemPanel();
    git_panel = new GitPanel();
    process_panel = new ProcessPanel();
    log_panel = new LogPanel(config.log_file);
    task_panel = new TaskPanel();
    initialized = true;
}

void DevPulseApp::refresh_loop() {
    while (running) {
        {
            std::lock_guard<std::mutex> lock(refresh_mutex);
            if (initialized) {
                sys_panel->refresh();
                git_panel->refresh();
                process_panel->refresh();
                log_panel->refresh();
            }
            pulse_state = (pulse_state + 1) % 4;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(config.refresh_rate_ms));
        if (running) {
            screen->PostEvent(Event::Custom);
        }
    }
}

std::string DevPulseApp::get_pulse_indicator() {
    const char* states[] = { " * ", " o ", " * ", " o " };
    return states[pulse_state];
}

std::string DevPulseApp::get_timestamp() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%H:%M:%S", &tm);
    return std::string(buf);
}

void DevPulseApp::run() {
    init_all();
    std::thread refresher([this] { refresh_loop(); });
    
    auto input_comp = Input(&input_buffer, "Type your task here...");
    
    auto renderer = Renderer([&] {
        std::lock_guard<std::mutex> lock(refresh_mutex);
        
        Elements left_elements = {
            sys_panel->render() | flex,
            process_panel->render() | flex,
        };
        Element left_column = vbox(left_elements) | flex;

        Elements right_elements = {
            git_panel->render() | flex,
            log_panel->render() | flex,
            task_panel->render() | flex,
        };
        Element right_column = vbox(right_elements) | flex;

        Color pulse_color = pulse_state % 2 == 0 ? Color::Green : Color::GreenLight;
        
        Elements header_els;
        header_els.push_back(text("  ") | flex);
        header_els.push_back(text("DevPulse") | bold | color(Color::CyanLight));
        header_els.push_back(text(" v1.0.0") | color(Color::White));
        header_els.push_back(text(" - Zenith Open Source Projects") | color(Color::GrayDark));
        header_els.push_back(text("              ") | flex);
        header_els.push_back(text("Live") | bold | color(pulse_color));
        header_els.push_back(text(" [") | color(Color::GrayDark));
        header_els.push_back(text("q") | color(Color::Red) | bold);
        header_els.push_back(text("] Quit") | color(Color::GrayDark));
        Element header = hbox(header_els);

        Element footer;
        if (input_mode) {
            Elements footer_els;
            footer_els.push_back(separator());
            footer_els.push_back(hbox(Elements{
                text("  ADD NEW TASK") | bold | color(Color::Yellow),
                text("                              ") | flex,
                text("ENTER: Save") | color(Color::Green),
                text("  |  ") | color(Color::GrayDark),
                text("ESC: Cancel") | color(Color::Red),
            }));
            footer_els.push_back(input_comp->Render() | border);
            footer = vbox(footer_els);
        } else {
            Elements footer_els;
            footer_els.push_back(separator());
            footer_els.push_back(hbox(Elements{
                text("  Copyright (c) 2024 ") | color(Color::GrayDark),
                text("Zenith Open Source Projects") | bold | color(Color::White),
                text("  |  All rights reserved") | color(Color::GrayDark),
                text("                    ") | flex,
                text("[q] Quit") | color(Color::Red) | bold,
                text("  [a] Add  ") | color(Color::Green),
                text("[d] Toggle  ") | color(Color::Yellow),
                text("[x] Delete  ") | color(Color::Red),
                text("[Up/Dn] Navigate") | color(Color::Cyan),
            }));
            footer_els.push_back(hbox(Elements{
                text("  DevPulse - Your Development Companion") | color(Color::GrayDark),
                text("                                         ") | flex,
                text("Last update: ") | color(Color::GrayDark),
                text(get_timestamp()) | color(Color::Cyan) | bold,
            }));
            footer = vbox(footer_els);
        }

        Elements main_els;
        main_els.push_back(header);
        main_els.push_back(hbox(Elements{
            left_column,
            text("|") | color(Color::GrayDark),
            right_column,
        }) | flex);
        main_els.push_back(footer);
        return vbox(main_els);
    });

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q')) {
            running = false;
            screen->ExitLoopClosure()();
            return true;
        }
        
        if (input_mode) {
            if (event == Event::Escape) {
                input_mode = false;
                input_buffer = "";
                return true;
            }
            if (event == Event::Return) {
                if (!input_buffer.empty()) {
                    task_panel->add_task(input_buffer);
                }
                input_mode = false;
                input_buffer = "";
                return true;
            }
            if (event == Event::Delete) {
                if (!input_buffer.empty()) {
                    input_buffer.pop_back();
                }
                return true;
            }
            if (event.is_character()) {
                input_buffer += event.character();
                return true;
            }
            return true;
        }
        
        if (event == Event::ArrowUp) {
            int sel = task_panel->get_selected();
            task_panel->set_selected(sel - 1);
            return true;
        }
        if (event == Event::ArrowDown) {
            int sel = task_panel->get_selected();
            task_panel->set_selected(sel + 1);
            return true;
        }
        if (event == Event::Character('a')) {
            input_mode = true;
            input_buffer = "";
            return true;
        }
        if (event == Event::Character('d')) {
            task_panel->toggle_task(task_panel->get_selected());
            return true;
        }
        if (event == Event::Character('x')) {
            task_panel->delete_task(task_panel->get_selected());
            return true;
        }
        return false;
    });

    screen->Loop(component);
    refresher.join();
}
