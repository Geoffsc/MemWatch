#include "meminfo.h"
#include <fstream>
#include <sstream>
#include <string>
#include <dirent.h>
#include <algorithm>
#include <cctype>  // for isdigit

MemStats MemInfo::getMemoryStats() {
    std::ifstream meminfo("/proc/meminfo");
    std::string line;
    long memTotal = 0, memFree = 0, memAvailable = 0;

    while (std::getline(meminfo, line)) {
        std::istringstream iss(line);
        std::string key;
        long value;
        std::string unit;
        iss >> key >> value >> unit;

        if (key == "MemTotal:") memTotal = value;
        else if (key == "MemFree:") memFree = value;
        else if (key == "MemAvailable:") memAvailable = value;

        if (memTotal && memFree && memAvailable) break;
    }

    long used = memTotal - memAvailable;
    double usage = memTotal > 0 ? (double)used / memTotal * 100.0 : 0;

    return MemStats{memTotal, memFree, memAvailable, used, usage};
}

std::vector<ProcessInfo> getTopProcessesByMemory(int count) {
    std::vector<ProcessInfo> processes;
    DIR* proc = opendir("/proc");
    if (!proc) return processes;

    struct dirent* ent;
    while ((ent = readdir(proc)) != nullptr) {
        if (!isdigit(ent->d_name[0])) continue;

        int pid = atoi(ent->d_name);
        std::string statusPath = "/proc/" + std::to_string(pid) + "/status";

        std::ifstream statusFile(statusPath);
        if (!statusFile) continue;

        std::string line;
        std::string name;
        int rss = 0;

        while (std::getline(statusFile, line)) {
            if (line.rfind("Name:", 0) == 0) {
                name = line.substr(6);
                name.erase(0, name.find_first_not_of(" \t")); // trim
            } else if (line.rfind("VmRSS:", 0) == 0) {
                std::istringstream iss(line.substr(6));
                iss >> rss; // in KB
                break;
            }
        }

        if (rss > 0) {
            processes.push_back({pid, name, rss});
        }
    }
    closedir(proc);

    std::sort(processes.begin(), processes.end(), [](const ProcessInfo& a, const ProcessInfo& b) {
        return a.memUsage > b.memUsage;
    });

    if ((int)processes.size() > count)
        processes.resize(count);

    return processes;
}
