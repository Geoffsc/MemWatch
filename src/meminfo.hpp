#pragma once
#include <string>

struct MemStats {
    long totalKB = 0;
    long availableKB = 0;
    double usagePercent = 0.0;
};

class MemInfo {
public:
    static MemStats getMemoryStats();
};
