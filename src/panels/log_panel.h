/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#pragma once
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>
#include <fstream>

class LogPanel {
public:
    LogPanel(const std::string& log_path = "/var/log/syslog");
    void refresh();
    ftxui::Element render();
    void set_log_path(const std::string& path);
private:
    std::string log_path;
    std::vector<std::string> lines;
    static const size_t MAX_LINES = 20;
    std::streampos last_pos;
    std::ifstream log_file;
};
