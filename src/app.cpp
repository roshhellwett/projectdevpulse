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
    : screen(ScreenInteractive::Fullscreen()),
      config(ConfigLoader().get_config()),
      running(true),
      input_mode(false),
      input_buffer(""),
      pulse_state(0) {
    log_panel.set_log_path(config.log_file);
}

DevPulseApp::~DevPulseApp() {
    running = false;
}

void DevPulseApp::refresh_loop() {
    while (running) {
        {
            std::lock_guard<std::mutex> lock(refresh_mutex);
            sys_panel.refresh();
            git_panel.refresh();
            process_panel.refresh();
            log_panel.refresh();
            pulse_state = (pulse_state + 1) % 4;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(config.refresh_rate_ms));
        if (running) {
            screen.PostEvent(Event::Custom);
        }
    }
}

std::string DevPulseApp::get_pulse_indicator() {
    const char* states[] = { " ● ", " ○ ", " ● ", " ○ " };
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
    std::thread refresher([this] { refresh_loop(); });
    
    auto input_comp = Input(&input_buffer, "Type your task here...");
    
    auto renderer = Renderer([&] {
        std::lock_guard<std::mutex> lock(refresh_mutex);
        
        Elements left_elements = {
            sys_panel.render() | flex,
            process_panel.render() | flex,
        };
        Element left_column = vbox(left_elements) | flex;

        Elements right_elements = {
            git_panel.render() | flex,
            log_panel.render() | flex,
            task_panel.render() | flex,
        };
        Element right_column = vbox(right_elements) | flex;

        Element header = hbox({
            text("") | flex,
            vbox({
                text("") | size(HEIGHT, EQUAL, 1),
                text("  ╔══════════════════════════════════════════════════════════╗") | color(Color::CyanLight),
                text("  ║") | color(Color::CyanLight),
                hbox({
                    text("  ║       ") | color(Color::CyanLight),
                    text("PROJECT DEV PULSE") | bold | color(Color::CyanLight),
                    text("  v1.0.0") | color(Color::White),
                }),
                text("  ║       ") | color(Color::CyanLight),
                hbox({
                    text("  ║       ") | color(Color::CyanLight),
                    text("Zenith Open Source Projects") | color(Color::White),
                    text("           ") | flex,
                    text("Live") | bold | color(pulse_state % 2 == 0 ? Color::Green : Color::GreenLight),
                }) | flex,
                text("  ╚══════════════════════════════════════════════════════════╝") | color(Color::CyanLight),
                text("") | size(HEIGHT, EQUAL, 1),
            }) | flex,
            text("") | flex,
        });

        Element footer;
        if (input_mode) {
            footer = vbox({
                separator() | color(Color::Yellow),
                hbox({
                    text("  ADD NEW TASK") | bold | color(Color::Yellow),
                    text("                              ") | flex,
                    text("ENTER: Save") | color(Color::Green),
                    text("  |  ") | color(Color::DimWhite),
                    text("ESC: Cancel") | color(Color::Red),
                }) | bold,
                input_comp->Render() | border,
            }) | color(Color::Yellow);
        } else {
            footer = vbox({
                separator() | color(Color::CyanDark),
                hbox({
                    text("  Copyright (c) 2024 ") | color(Color::DimWhite),
                    text("Zenith Open Source Projects") | bold | color(Color::White),
                    text("  |  All rights reserved") | color(Color::DimWhite),
                    text("                    ") | flex,
                    text("[q] Quit") | color(Color::Red) | bold,
                    text("  [a] Add  ") | color(Color::Green),
                    text("[d] Toggle  ") | color(Color::Yellow),
                    text("[x] Delete  ") | color(Color::Red),
                    text("[↑↓] Navigate") | color(Color::Cyan),
                }),
                hbox({
                    text("  DevPulse - Your Development Companion") | color(Color::DimWhite),
                    text("                                         ") | flex,
                    text("Last update: ") | color(Color::DimWhite),
                    text(get_timestamp()) | color(Color::Cyan) | bold,
                }),
            });
        }

        return vbox({
            header,
            hbox({
                left_column,
                text("│") | color(Color::CyanDark),
                right_column,
            }) | flex,
            footer,
        });
    });

    auto component = CatchEvent(renderer, [&](Event event) {
        if (event == Event::Character('q')) {
            running = false;
            screen.ExitLoopClosure()();
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
                    task_panel.add_task(input_buffer);
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
            int sel = task_panel.get_selected();
            task_panel.set_selected(sel - 1);
            return true;
        }
        if (event == Event::ArrowDown) {
            int sel = task_panel.get_selected();
            task_panel.set_selected(sel + 1);
            return true;
        }
        if (event == Event::Character('a')) {
            input_mode = true;
            input_buffer = "";
            return true;
        }
        if (event == Event::Character('d')) {
            task_panel.toggle_task(task_panel.get_selected());
            return true;
        }
        if (event == Event::Character('x')) {
            task_panel.delete_task(task_panel.get_selected());
            return true;
        }
        return false;
    });

    screen.Loop(component);
    refresher.join();
}
