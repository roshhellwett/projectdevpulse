/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "system_panel.h"
#include <ftxui/dom/elements.hpp>
#include <string>
#include <iomanip>
#include <sstream>

using namespace ftxui;

namespace {
    std::string format_number(float n) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << n;
        return ss.str();
    }
    
    Color get_usage_color(float pct) {
        if (pct < 50.0f) return Color::Green;
        if (pct < 80.0f) return Color::Yellow;
        return Color::Red;
    }
    
    Element make_bar(float pct, const std::string& label) {
        int total_width = 14;
        int filled = (int)(pct / 100.0f * total_width);
        if (filled > total_width) filled = total_width;
        if (filled < 0) filled = 0;
        
        Color bar_color = get_usage_color(pct);
        
        return hbox(Elements{
            text(label) | color(Color::Cyan),
            text(" [") | color(Color::White),
            text(std::string(filled, '#')) | color(bar_color),
            text(std::string(total_width - filled, '-')) | color(Color::GrayDark),
            text("] ") | color(Color::White),
        });
    }
}

SystemPanel::SystemPanel() { refresh(); }

void SystemPanel::refresh() {
    stats = read_system_stats();
}

Element SystemPanel::render() {
    Elements els;
    els.push_back(hbox(Elements{
        text(" SYSTEM MONITOR ") | bold | color(Color::Cyan) | inverted,
    }));
    els.push_back(separator());
    
    els.push_back(make_bar(stats.cpu_percent, " CPU"));
    els.push_back(hbox(Elements{
        text("    ") | flex,
        text(format_number(stats.cpu_percent) + "%") | bold | color(get_usage_color(stats.cpu_percent)),
    }));
    
    els.push_back(make_bar(stats.ram_percent, " RAM"));
    els.push_back(hbox(Elements{
        text("    ") | flex,
        text(format_number((float)stats.ram_used_mb) + " / " + 
             format_number((float)stats.ram_total_mb) + " MB") | bold | color(get_usage_color(stats.ram_percent)),
    }));
    
    els.push_back(make_bar(stats.disk_percent, " DISK"));
    els.push_back(hbox(Elements{
        text("    ") | flex,
        text(format_number((float)stats.disk_used_gb) + " / " + 
             format_number((float)stats.disk_total_gb) + " GB") | bold | color(get_usage_color(stats.disk_percent)),
    }));
    
    els.push_back(separator());
    els.push_back(text(""));
    return vbox(els) | border;
}
