#include "notifier.hpp"
#include <cstdlib>
#include <iostream>

Notifier::Notifier(bool enableNotify, bool enableSpeech)
    : enabledNotify(enableNotify), enabledSpeech(enableSpeech) {
    notifySendAvailable = checkCommandAvailable("notify-send");
    espeakAvailable = checkCommandAvailable("espeak");
}

bool Notifier::checkCommandAvailable(const std::string& cmd) {
    std::string check = "which " + cmd + " > /dev/null 2>&1";
    return system(check.c_str()) == 0;
}

void Notifier::enableNotifications(bool enable) {
    enabledNotify = enable;
}

void Notifier::enableSpeech(bool enable) {
    enabledSpeech = enable;
}

void Notifier::send(const std::string& title, const std::string& message) {
    if (enabledNotify && notifySendAvailable) {
        std::string cmd = "notify-send \"" + title + "\" \"" + message + "\"";
        if (system(cmd.c_str()) != 0) {
            std::cerr << "Warning: Failed to send notify-send alert.\n";
        }
    }

    if (enabledSpeech && espeakAvailable) {
        std::string speakCmd = "espeak \"" + message + "\"";
        if (system(speakCmd.c_str()) != 0) {
            std::cerr << "Warning: Failed to speak alert.\n";
        }
    }
}
