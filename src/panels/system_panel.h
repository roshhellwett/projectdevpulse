/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#pragma once
#include <ftxui/dom/elements.hpp>
#include <string>

class SystemPanel {
public:
    SystemPanel();
    void refresh();
    ftxui::Element render();
private:
    struct SystemStats {
        float cpu_percent = 0.0f;
        long ram_used_mb = 0;
        long ram_total_mb = 0;
        float ram_percent = 0.0f;
        long disk_used_gb = 0;
        long disk_total_gb = 0;
        float disk_percent = 0.0f;
    };
    SystemStats stats;
    std::string cached_hostname;
};
