/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "system_panel.h"
#include <ftxui/dom/elements.hpp>
#include <string>

using namespace ftxui;

SystemPanel::SystemPanel() { refresh(); }

void SystemPanel::refresh() {
    stats = read_system_stats();
}

Element SystemPanel::render() {
    auto bar = [](float pct, Color col) {
        int filled = (int)(pct / 5.0f); // 20 chars wide
        if (filled > 20) filled = 20;
        return hbox({
            text("[") | color(Color::White),
            text(std::string(filled, '|')) | color(col),
            text(std::string(20 - filled, ' ')),
            text("]") | color(Color::White),
        });
    };

    return vbox({
        text("SYSTEM") | bold | color(Color::Cyan),
        separator(),
        hbox({ text("CPU : "),
               bar(stats.cpu_percent, Color::Green),
               text(std::string(" ") +
                    std::to_string((int)stats.cpu_percent) + "%") }),
        hbox({ text("RAM : "),
               bar(stats.ram_percent, Color::Yellow),
               text(std::string(" ") +
                    std::to_string(stats.ram_used_mb) + "/" +
                    std::to_string(stats.ram_total_mb) + "MB") }),
        hbox({ text("DISK: "),
               bar(stats.disk_percent, Color::Magenta),
               text(std::string(" ") +
                    std::to_string(stats.disk_used_gb) + "/" +
                    std::to_string(stats.disk_total_gb) + "GB") }),
    }) | border;
}
