/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#pragma once
#include <string>
#include <vector>

struct Config {
    int refresh_rate_ms = 1000;
    std::string theme = "dark";
    
    bool show_system = true;
    bool show_git = true;
    bool show_process = true;
    bool show_log = true;
    bool show_tasks = true;
    
    std::string log_file = "devpulse.log";
    std::vector<std::string> process_filter = {
        "gcc", "g++", "python", "node", "vim", "nvim", "make"
    };
};

class ConfigLoader {
public:
    ConfigLoader(const std::string& path = "devpulse.toml");
    Config get_config() const { return config; }
private:
    Config config;
    void parse_line(const std::string& line);
    std::string trim(const std::string& s);
};
