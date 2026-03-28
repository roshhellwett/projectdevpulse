/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "app.h"
#include <ftxui/dom/elements.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/input.hpp>
#include <thread>
#include <chrono>
#include <mutex>

using namespace ftxui;

static std::mutex refresh_mutex;

DevPulseApp::DevPulseApp() 
    : screen(ScreenInteractive::Fullscreen()),
      config(ConfigLoader().get_config()),
      running(true),
      input_mode(false),
      input_field(Input({
          .placeholder = "Type task and press ENTER...",
          .on_change = [] {},
          .on_enter = [&] {
              std::string text = input_field.content();
              if (!text.empty() && input_mode) {
                  task_panel.add_task(text);
              }
              input_mode = false;
              input_field.content() = "";
          },
      })) {
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
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(config.refresh_rate_ms));
        if (running) {
            screen.PostEvent(Event::Custom);
        }
    }
}

void DevPulseApp::run() {
    std::thread refresher([this] { refresh_loop(); });
    
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

        Element footer;
        if (input_mode) {
            footer = vbox({
                text("New Task:") | bold | color(Color::Yellow),
                input_field.Render() | border,
                text("ENTER: save | ESC: cancel") | color(Color::Cyan),
            }) | center;
        } else {
            footer = vbox({
                text("q: quit | a: add task | d: toggle | x: delete | arrows: navigate") | color(Color::Cyan),
            }) | center;
        }

        return vbox({
            text("devpulse - Zenith Open Source Projects") | bold | center,
            separator(),
            hbox({
                left_column,
                separator(),
                right_column,
            }) | flex,
            separator(),
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
                input_field.content() = "";
                return true;
            }
            return false;
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
            input_field.content() = "";
            input_field.TakeFocus() = true;
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
