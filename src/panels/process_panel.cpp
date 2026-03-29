/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "process_panel.h"
#include <ftxui/dom/elements.hpp>
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <cstring>
#include <algorithm>
#include <vector>

using namespace ftxui;

static const char* DEV_PROCESSES[] = {
    "gcc", "g++", "clang", "clang++", "python", "python3", "python2", "node", "npm",
    "yarn", "vim", "nvim", "emacs", "make", "cmake", "cargo", "rustc",
    "go", "javac", "java", "perl", "ruby", "php", "lua", "dotnet", "gradle",
    "webpack", "vite", "next", "nuxt", "django", "flask", "rails", "spring",
    "mvn", "sbt", "zig", "swift", "kotlin", "scala",
    "code", "devenv", "clion", "pycharm", "webstorm", "goland",
    "sublime_text", "notepad++", "git", "cmd", "powershell", "terminal",
    "devpulse"
};

static const size_t NUM_DEV_PROCESSES = sizeof(DEV_PROCESSES) / sizeof(DEV_PROCESSES[0]);

bool ProcessPanel::is_dev_process(const std::string& name) {
    std::string lower_name = name;
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), ::tolower);
    for (size_t i = 0; i < NUM_DEV_PROCESSES; ++i) {
        if (lower_name.find(DEV_PROCESSES[i]) != std::string::npos)
            return true;
    }
    return false;
}

std::vector<ProcessInfo> ProcessPanel::get_dev_processes() {
    std::vector<ProcessInfo> result;
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE)
        return result;
    
    FILETIME curr_idle, curr_kernel, curr_user;
    GetSystemTimes(&curr_idle, &curr_kernel, &curr_user);
    
    ULONGLONG system_delta = 0;
    
    if (!first_run) {
        ULARGE_INTEGER ul_kernel, ul_user, ul_last_kernel, ul_last_user;
        memcpy(&ul_kernel, &curr_kernel, sizeof(ULARGE_INTEGER));
        memcpy(&ul_user, &curr_user, sizeof(ULARGE_INTEGER));
        memcpy(&ul_last_kernel, &last_kernel_time, sizeof(ULARGE_INTEGER));
        memcpy(&ul_last_user, &last_user_time, sizeof(ULARGE_INTEGER));
        
        ULONGLONG kern_delta = ul_kernel.QuadPart - ul_last_kernel.QuadPart;
        ULONGLONG user_delta = ul_user.QuadPart - ul_last_user.QuadPart;
        system_delta = kern_delta + user_delta;
    }
    
    memcpy(&last_idle_time, &curr_idle, sizeof(ULARGE_INTEGER));
    memcpy(&last_kernel_time, &curr_kernel, sizeof(ULARGE_INTEGER));
    memcpy(&last_user_time, &curr_user, sizeof(ULARGE_INTEGER));
    first_run = false;
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(snapshot, &pe32)) {
        do {
            std::string process_name = pe32.szExeFile;
            if (is_dev_process(process_name)) {
                ProcessInfo info;
                info.pid = pe32.th32ProcessID;
                info.name = process_name;
                
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
                if (hProcess) {
                    PROCESS_MEMORY_COUNTERS pmc;
                    if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                        info.memory_mb = pmc.WorkingSetSize / (1024 * 1024);
                    }
                    
                    FILETIME ft_creation, ft_exit, ft_kernel, ft_user;
                    if (GetProcessTimes(hProcess, &ft_creation, &ft_exit, &ft_kernel, &ft_user)) {
                        ULARGE_INTEGER ul_kernel, ul_user;
                        memcpy(&ul_kernel, &ft_kernel, sizeof(ULARGE_INTEGER));
                        memcpy(&ul_user, &ft_user, sizeof(ULARGE_INTEGER));
                        ULONGLONG process_time = ul_kernel.QuadPart + ul_user.QuadPart;
                        if (system_delta > 0) {
                            info.cpu_percent = (double)process_time / system_delta * 100.0;
                            if (info.cpu_percent > 100.0) info.cpu_percent = 100.0;
                        }
                    }
                    CloseHandle(hProcess);
                }
                
                result.push_back(info);
            }
        } while (Process32Next(snapshot, &pe32));
    }
    
    CloseHandle(snapshot);
    
    std::sort(result.begin(), result.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        return a.name < b.name;
    });
    
    return result;
}

ProcessPanel::ProcessPanel() {
    last_idle_time.QuadPart = 0;
    last_kernel_time.QuadPart = 0;
    last_user_time.QuadPart = 0;
    refresh();
}

void ProcessPanel::refresh() {
    processes = get_dev_processes();
}

Element ProcessPanel::render() {
    Elements els;
    
    size_t count = processes.size();
    
    els.push_back(text("PROCESSES: " + std::to_string(count) + " running") | bold | color(Color::Magenta));
    
    if (processes.empty()) {
        els.push_back(text("No dev processes running") | color(Color::GrayDark));
    } else {
        for (auto& p : processes) {
            std::string name = p.name;
            if (name.length() > 15) {
                name = name.substr(0, 12) + "...";
            }
            
            std::string info = name + " | PID: " + std::to_string(p.pid) + 
                             " | CPU: " + std::to_string((int)p.cpu_percent) + "%" +
                             " | MEM: " + std::to_string(p.memory_mb) + "MB";
            els.push_back(text(info) | color(Color::White));
        }
    }
    
    return vbox(els) | border;
}
