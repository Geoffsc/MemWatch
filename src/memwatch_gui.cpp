#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Multiline_Output.H>
#include <atomic>
#include <thread>
#include <string>
#include "meminfo.h"

// Globals
std::atomic<int> threshold(80);
Fl_Box* thresholdValueLabel = nullptr;
Fl_Box* infoBox = nullptr;
Fl_Multiline_Output* processBox = nullptr;
std::string thresholdText;

void checkMemory() {
    while (true) {
        auto [total, free, available, used, usage] = MemInfo::getMemoryStats();
        int usedPercent = 100 - (available * 100 / total);

        std::string usageText = "Usage: " + std::to_string(usedPercent) + "%";

        Fl::lock();
        if (infoBox) {
            infoBox->label(usageText.c_str());
            infoBox->redraw();
        }
        if (usedPercent != 0 && usedPercent >= threshold.load()) {
            infoBox->labelcolor(FL_RED);
            system("espeak 'Memory usage exceeded threshold'");
        } else {
            infoBox->labelcolor(FL_BLACK);
        }

        // Display top memory-using processes
        auto topProcs = getTopProcessesByMemory(5);
        std::string procOutput = "";
        for (const auto& proc : topProcs) {
            procOutput += std::to_string(proc.pid) + " " + proc.name + " " + std::to_string(proc.memUsage / 1024) + " MB\n";
        }

        if (processBox) {
            processBox->value(procOutput.c_str());
            processBox->redraw();
        }

        Fl::unlock();
        Fl::awake();

        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
}

void slider_callback(Fl_Widget* w, void*) {
    Fl_Slider* s = (Fl_Slider*)w;
    threshold = static_cast<int>(s->value());
    thresholdText = "Current Threshold: " + std::to_string(threshold.load()) + "%";
    thresholdValueLabel->label(thresholdText.c_str());
    thresholdValueLabel->redraw();
}

int main(int argc, char** argv) {
    Fl::lock();
    Fl_Window* window = new Fl_Window(340, 400, "MemWatch GUI");

    // Label for slider
    Fl_Box* label = new Fl_Box(20, 20, 300, 25, "Set memory usage alert threshold:");
    label->labelfont(FL_BOLD);
    label->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);

    // Slider
    Fl_Slider* slider = new Fl_Slider(20, 50, 300, 25);
    slider->type(FL_HOR_NICE_SLIDER);
    slider->bounds(50, 100);  // min=50%, max=100%
    slider->value(threshold);
    slider->callback(slider_callback);

    // Label showing threshold value
    thresholdText = "Current Threshold: " + std::to_string(threshold.load()) + "%";
    thresholdValueLabel = new Fl_Box(20, 80, 300, 20, thresholdText.c_str());
    thresholdValueLabel->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    thresholdValueLabel->labelfont(FL_ITALIC);

    // Memory usage info box
    infoBox = new Fl_Box(20, 110, 300, 40, "Usage:");
    infoBox->box(FL_UP_BOX);
    infoBox->labelsize(20);

    // Top memory-consuming processes box
    Fl_Box* processLabel = new Fl_Box(20, 160, 280, 20, "Top Memory Processes:");
    processLabel->labelfont(FL_BOLD);
    processLabel->align(FL_ALIGN_CENTER);
    processBox = new Fl_Multiline_Output(20, 185, 280, 120);
    processBox->textfont(FL_COURIER);


    window->end();
    window->show(argc, argv);

    std::thread memThread(checkMemory);
    memThread.detach();  // Don't block on exit
    return Fl::run();
}
