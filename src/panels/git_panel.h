/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#pragma once
#include <ftxui/dom/elements.hpp>
#include <string>
#include <vector>

struct GitInfo {
    bool is_git = false;
    std::string branch;
    std::vector<std::string> status;
    std::vector<std::string> commits;
};

class GitPanel {
public:
    GitPanel();
    void refresh();
    ftxui::Element render();
private:
    GitInfo info;
};
