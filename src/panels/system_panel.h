/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#pragma once
#include <ftxui/dom/elements.hpp>

extern "C" {
    #include "sysreader.h"
}

class SystemPanel {
public:
    SystemPanel();
    void refresh();
    ftxui::Element render();
private:
    SystemStats stats;
};
