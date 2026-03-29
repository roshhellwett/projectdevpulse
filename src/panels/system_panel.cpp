/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "system_panel.h"
#include <ftxui/dom/elements.hpp>
#include <string>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <algorithm>
#include <cstdio>

using namespace ftxui;

namespace {
    std::string format_number(float n) {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << n;
        return ss.str();
    }
    
    std::string get_hostname_cached() {
        char hostname[256];
        DWORD size = 256;
        if (GetComputerNameA(hostname, &size)) {
            return std::string(hostname);
        }
        return "unknown";
    }
    
    std::string get_uptime() {
        ULONGLONG uptime_ms = GetTickCount64();
        int hours = (uptime_ms / 1000 / 60 / 60) % 24;
        int minutes = (uptime_ms / 1000 / 60) % 60;
        int days = uptime_ms / 1000 / 60 / 60 / 24;
        if (days > 0) {
            return std::to_string(days) + "d " + std::to_string(hours) + "h";
        }
        return std::to_string(hours) + "h " + std::to_string(minutes) + "m";
    }
    
    static ULARGE_INTEGER last_idle_time = {0};
    static ULARGE_INTEGER last_kernel_time = {0};
    static ULARGE_INTEGER last_user_time = {0};
    static bool sys_initialized = false;
    
    float read_cpu_usage() {
        FILETIME idle_time, kernel_time, user_time;
        if (!GetSystemTimes(&idle_time, &kernel_time, &user_time)) {
            return 0.0f;
        }
        
        ULARGE_INTEGER idle, kernel, user;
        idle.QuadPart = idle_time.dwHighDateTime;
        idle.QuadPart = (idle.QuadPart << 32) | idle_time.dwLowDateTime;
        kernel.QuadPart = kernel_time.dwHighDateTime;
        kernel.QuadPart = (kernel.QuadPart << 32) | kernel_time.dwLowDateTime;
        user.QuadPart = user_time.dwHighDateTime;
        user.QuadPart = (user.QuadPart << 32) | user_time.dwLowDateTime;
        
        if (!sys_initialized) {
            last_idle_time = idle;
            last_kernel_time = kernel;
            last_user_time = user;
            sys_initialized = true;
            return 0.0f;
        }
        
        ULONGLONG idle_delta = idle.QuadPart - last_idle_time.QuadPart;
        ULONGLONG kernel_delta = kernel.QuadPart - last_kernel_time.QuadPart;
        ULONGLONG user_delta = user.QuadPart - last_user_time.QuadPart;
        ULONGLONG total_delta = kernel_delta + user_delta + idle_delta;
        
        last_idle_time = idle;
        last_kernel_time = kernel;
        last_user_time = user;
        
        if (total_delta == 0) return 0.0f;
        
        float cpu = 100.0f * (1.0f - (float)idle_delta / (float)total_delta);
        if (cpu < 0.0f) cpu = 0.0f;
        if (cpu > 100.0f) cpu = 100.0f;
        return cpu;
    }
    
    void read_memory(long* used_mb, long* total_mb) {
        MEMORYSTATUSEX mem_info;
        mem_info.dwLength = sizeof(mem_info);
        if (GlobalMemoryStatusEx(&mem_info)) {
            *total_mb = (long)(mem_info.ullTotalPhys / (1024 * 1024));
            *used_mb = (long)((mem_info.ullTotalPhys - mem_info.ullAvailPhys) / (1024 * 1024));
        } else {
            *total_mb = 0;
            *used_mb = 0;
        }
    }
    
    void read_disk(long* used_gb, long* total_gb) {
        ULARGE_INTEGER free_bytes, total_bytes, total_free;
        if (GetDiskFreeSpaceExW(L"C:\\", &free_bytes, &total_bytes, &total_free)) {
            *total_gb = (long)(total_bytes.QuadPart / (1024ULL * 1024ULL * 1024ULL));
            *used_gb = (long)((total_bytes.QuadPart - free_bytes.QuadPart) / (1024ULL * 1024ULL * 1024ULL));
        } else {
            *total_gb = 0;
            *used_gb = 0;
        }
    }
}

SystemPanel::SystemPanel() 
    : cached_hostname(get_hostname_cached()) {
    refresh();
}

void SystemPanel::refresh() {
    stats.cpu_percent = read_cpu_usage();
    read_memory(&stats.ram_used_mb, &stats.ram_total_mb);
    read_disk(&stats.disk_used_gb, &stats.disk_total_gb);
    stats.ram_percent = (stats.ram_total_mb > 0) ?
        (float)stats.ram_used_mb / stats.ram_total_mb * 100.0f : 0.0f;
    stats.disk_percent = (stats.disk_total_gb > 0) ?
        (float)stats.disk_used_gb / stats.disk_total_gb * 100.0f : 0.0f;
}

Element SystemPanel::render() {
    Elements els;
    
    els.push_back(text("SYSTEM: Host " + cached_hostname));
    
    int cpu_bar = (int)(stats.cpu_percent / 10);
    std::string cpu_bar_str = "[";
    for (int i = 0; i < 10; i++) {
        cpu_bar_str += (i < cpu_bar) ? "#" : "-";
    }
    cpu_bar_str += "]";
    els.push_back(text("CPU: " + cpu_bar_str + " " + format_number(stats.cpu_percent) + "%") | color(Color::Green));
    
    int ram_bar = (int)(stats.ram_percent / 10);
    std::string ram_bar_str = "[";
    for (int i = 0; i < 10; i++) {
        ram_bar_str += (i < ram_bar) ? "#" : "-";
    }
    ram_bar_str += "]";
    els.push_back(text("RAM: " + ram_bar_str + " " + format_number((float)stats.ram_used_mb) + 
                       " / " + format_number((float)stats.ram_total_mb) + " MB") | color(Color::Cyan));
    
    int disk_bar = (int)(stats.disk_percent / 10);
    std::string disk_bar_str = "[";
    for (int i = 0; i < 10; i++) {
        disk_bar_str += (i < disk_bar) ? "#" : "-";
    }
    disk_bar_str += "]";
    els.push_back(text("Disk: " + disk_bar_str + " " + format_number((float)stats.disk_used_gb) + 
                       " / " + format_number((float)stats.disk_total_gb) + " GB") | color(Color::Magenta));
    
    els.push_back(text("Uptime: " + get_uptime()));
    
    return vbox(els) | border;
}
