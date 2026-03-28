/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#include "sysreader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/statvfs.h>

/* CPU usage via /proc/stat */
float read_cpu_usage(void) {
    static long prev_idle = 0, prev_total = 0;
    FILE *f = fopen("/proc/stat", "r");
    if (!f) return 0.0f;

    long user, nice, system, idle, iowait, irq, softirq;
    fscanf(f, "cpu %ld %ld %ld %ld %ld %ld %ld",
           &user, &nice, &system, &idle, &iowait, &irq, &softirq);
    fclose(f);

    long total = user + nice + system + idle + iowait + irq + softirq;
    long diff_idle  = idle - prev_idle;
    long diff_total = total - prev_total;

    float cpu = (diff_total == 0) ? 0.0f :
        (float)(diff_total - diff_idle) / diff_total * 100.0f;

    prev_idle  = idle;
    prev_total = total;
    return cpu;
}

/* Memory via /proc/meminfo */
void read_memory(long *used_mb, long *total_mb) {
    FILE *f = fopen("/proc/meminfo", "r");
    if (!f) { *used_mb = 0; *total_mb = 0; return; }

    long total_kb = 0, free_kb = 0, buffers_kb = 0, cached_kb = 0;
    char key[64]; long value;

    while (fscanf(f, "%s %ld kB", key, &value) == 2) {
        if (strcmp(key, "MemTotal:") == 0) total_kb = value;
        else if (strcmp(key, "MemFree:") == 0) free_kb = value;
        else if (strcmp(key, "Buffers:") == 0) buffers_kb = value;
        else if (strcmp(key, "Cached:") == 0) cached_kb = value;
    }
    fclose(f);

    long used_kb = total_kb - free_kb - buffers_kb - cached_kb;
    *total_mb = total_kb / 1024;
    *used_mb  = used_kb  / 1024;
}

/* Disk via statvfs */
void read_disk(long *used_gb, long *total_gb) {
    struct statvfs stat;
    if (statvfs("/", &stat) != 0) {
        *used_gb = 0; *total_gb = 0; return;
    }
    long total_bytes = stat.f_blocks * stat.f_frsize;
    long free_bytes  = stat.f_bfree  * stat.f_frsize;
    *total_gb = total_bytes / (1024 * 1024 * 1024);
    *used_gb  = (total_bytes - free_bytes) / (1024 * 1024 * 1024);
}

/* Master function — call once per refresh */
SystemStats read_system_stats(void) {
    SystemStats s;
    s.cpu_percent = read_cpu_usage();
    read_memory(&s.ram_used_mb, &s.ram_total_mb);
    read_disk(&s.disk_used_gb, &s.disk_total_gb);
    s.ram_percent  = (s.ram_total_mb  > 0) ?
        (float)s.ram_used_mb  / s.ram_total_mb  * 100.0f : 0.0f;
    s.disk_percent = (s.disk_total_gb > 0) ?
        (float)s.disk_used_gb / s.disk_total_gb * 100.0f : 0.0f;
    return s;
}
