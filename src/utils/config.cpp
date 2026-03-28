/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "config.h"
#include <fstream>
#include <sstream>
#include <algorithm>

ConfigLoader::ConfigLoader(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return;
    
    std::string current_section;
    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;
        
        if (line[0] == '[') {
            size_t end = line.find(']');
            if (end != std::string::npos) {
                current_section = line.substr(1, end - 1);
            }
            continue;
        }
        
        size_t eq = line.find('=');
        if (eq != std::string::npos) {
            std::string key = trim(line.substr(0, eq));
            std::string val = trim(line.substr(eq + 1));
            
            if (current_section == "general") {
                if (key == "refresh_rate_ms") {
                    config.refresh_rate_ms = std::stoi(val);
                } else if (key == "theme") {
                    config.theme = val;
                }
            } else if (current_section == "panels") {
                if (key == "show_system") config.show_system = (val == "true");
                else if (key == "show_git") config.show_git = (val == "true");
                else if (key == "show_process") config.show_process = (val == "true");
                else if (key == "show_log") config.show_log = (val == "true");
                else if (key == "show_tasks") config.show_tasks = (val == "true");
            } else if (current_section == "log") {
                if (key == "file") config.log_file = val;
            }
        }
    }
}

std::string ConfigLoader::trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}
