#include "meminfo.hpp"
#include "notifier.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <cstdlib>
#include <cstring>

int main(int argc, char* argv[]) {
    int threshold = 85;
    int intervalSeconds = 5;
    bool enableNotify = false;
    bool enableSpeech = false;

    // Parse args
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--notify") {
            enableNotify = true;
            enableSpeech = true;
        } else if (std::string(argv[i]) == "--threshold" && i + 1 < argc) {
            threshold = std::stoi(argv[++i]);
        }
    }

    Notifier notifier(enableNotify, enableSpeech);

    std::cout << "memwatch: Monitoring RAM every " << intervalSeconds << "s (Threshold: "
              << threshold << "%, Notify: " << (enableNotify ? "on" : "off") << ")\n";

    while (true) {
        MemStats stats = MemInfo::getMemoryStats();

        std::cout << "RAM Usage: " << stats.usagePercent << "% (" 
                  << (stats.totalKB - stats.availableKB) / 1024 << " MB used / "
                  << stats.totalKB / 1024 << " MB total)\n";

        if (stats.usagePercent > threshold) {
            std::cout << "\n⚠️  Warning: High memory usage!\n";
            std::cout << "🔎 Consider closing memory-hungry applications or adding swap.\n";
            std::cout << "💡 Tip: Run `htop` or `top` to inspect memory usage.\n\n";

            notifier.send("High Memory Usage",
                          "Memory usage is " + std::to_string((int)stats.usagePercent) +
                          " percent. Consider closing apps.");
        }

        std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
    }

    return 0;
}
