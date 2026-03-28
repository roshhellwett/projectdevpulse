/**
 * @project  devpulse
 * @org      Zenith Open Source Projects
 * @license  MIT License
 */

#ifndef SYSREADER_H
#define SYSREADER_H

typedef struct {
    float cpu_percent;
    long  ram_used_mb;
    long  ram_total_mb;
    float ram_percent;
    long  disk_used_gb;
    long  disk_total_gb;
    float disk_percent;
} SystemStats;

/* Call this once per second to get updated stats */
SystemStats read_system_stats(void);

/* Internal helpers */
float read_cpu_usage(void);
void  read_memory(long *used_mb, long *total_mb);
void  read_disk(long *used_gb, long *total_gb);

#endif /* SYSREADER_H */
