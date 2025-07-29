#ifndef MEMINFO_H
#define MEMINFO_H

#include <string>
#include <vector>

struct MemStats {
    long total;
    long free;
    long available;
    long used;
    double usagePercent;
};

struct ProcessInfo {
    int pid;
    std::string name;
    int memUsage;
};

class MemInfo {
public:
    static MemStats getMemoryStats();
    // static std::tuple<long, long> getMemoryStats();
};

// Declaration only!
std::vector<ProcessInfo> getTopProcessesByMemory(int count);

#endif
