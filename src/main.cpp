#include <iostream>
#include <string>
#include "keylogger.h"
#include "security.h"

int main() {
    std::cout << "=== Educational Keylogger Demo ===" << std::endl;
    std::cout << "This is for EDUCATIONAL PURPOSES ONLY" << std::endl;
    
    // Get user consent
    std::cout << "Do you consent to this keylogger monitoring your keyboard? (yes/no): ";
    std::string consent;
    std::cin >> consent;
    
    if (consent != "yes") {
        std::cout << "Consent not given. Exiting..." << std::endl;
        return 0;
    }
    
    EducationalKeylogger logger;
    
    try {
        std::cout << "Starting educational keylogger (Press ESC to stop)..." << std::endl;
        logger.startMonitoring();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}