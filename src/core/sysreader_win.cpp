/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    float cpu_percent;
    long  ram_used_mb;
    long  ram_total_mb;
    float ram_percent;
    long  disk_used_gb;
    long  disk_total_gb;
    float disk_percent;
} SystemStats;

static ULARGE_INTEGER last_idle_time = {0};
static ULARGE_INTEGER last_kernel_time = {0};
static ULARGE_INTEGER last_user_time = {0};
static int initialized = 0;

float read_cpu_usage(void) {
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
    
    if (!initialized) {
        last_idle_time = idle;
        last_kernel_time = kernel;
        last_user_time = user;
        initialized = 1;
        return 0.0f;
    }
    
    ULONGLONG idle_delta = idle.QuadPart - last_idle_time.QuadPart;
    ULONGLONG kernel_delta = kernel.QuadPart - last_kernel_time.QuadPart;
    ULONGLONG user_delta = user.QuadPart - last_user_time.QuadPart;
    ULONGLONG total_delta = kernel_delta + user_delta + idle_delta;
    
    last_idle_time = idle;
    last_kernel_time = kernel;
    last_user_time = user;
    
    if (total_delta == 0) {
        return 0.0f;
    }
    
    float cpu = 100.0f * (1.0f - (float)idle_delta / (float)total_delta);
    
    if (cpu < 0.0f) cpu = 0.0f;
    if (cpu > 100.0f) cpu = 100.0f;
    
    return cpu;
}

void read_memory(long *used_mb, long *total_mb) {
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

void read_disk(long *used_gb, long *total_gb) {
    ULARGE_INTEGER free_bytes, total_bytes, total_free;
    
    if (GetDiskFreeSpaceExW(L"C:\\", &free_bytes, &total_bytes, &total_free)) {
        *total_gb = (long)(total_bytes.QuadPart / (1024ULL * 1024ULL * 1024ULL));
        *used_gb = (long)((total_bytes.QuadPart - free_bytes.QuadPart) / (1024ULL * 1024ULL * 1024ULL));
    } else {
        if (GetDiskFreeSpaceExW(NULL, &free_bytes, &total_bytes, &total_free)) {
            *total_gb = (long)(total_bytes.QuadPart / (1024ULL * 1024ULL * 1024ULL));
            *used_gb = (long)((total_bytes.QuadPart - free_bytes.QuadPart) / (1024ULL * 1024ULL * 1024ULL));
        } else {
            *total_gb = 0;
            *used_gb = 0;
        }
    }
}

SystemStats read_system_stats(void) {
    SystemStats s;
    s.cpu_percent = read_cpu_usage();
    read_memory(&s.ram_used_mb, &s.ram_total_mb);
    read_disk(&s.disk_used_gb, &s.disk_total_gb);
    s.ram_percent = (s.ram_total_mb > 0) ?
        (float)s.ram_used_mb / s.ram_total_mb * 100.0f : 0.0f;
    s.disk_percent = (s.disk_total_gb > 0) ?
        (float)s.disk_used_gb / s.disk_total_gb * 100.0f : 0.0f;
    return s;
}

#ifdef __cplusplus
}
#endif
