#pragma once
#include <string>

class Notifier {
public:
    Notifier(bool enableNotify = false, bool enableSpeech = false);
    void send(const std::string& title, const std::string& message);
    void enableNotifications(bool enable);
    void enableSpeech(bool enable);

private:
    bool enabledNotify;
    bool enabledSpeech;
    bool notifySendAvailable;
    bool espeakAvailable;

    bool checkCommandAvailable(const std::string& cmd);
};
