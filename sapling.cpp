#include <format>
#include <filesystem>

#include "sapling.h"

Sapling::Sapling(std::string LogFilePath, bool enableConsoleLogging, bool enableColor, bool enableTimestamping) {
    this->LogFilePath = LogFilePath;
    this->enableConsoleLogging = enableConsoleLogging;
    this->enableColor = enableColor;
    this->enableTimestamping = enableTimestamping;
}

void Sapling::log(const std::string &message, LogLevel level, std::string OneTimeLogFilePath,
    const std::source_location location) {
        std::lock_guard<std::mutex> lock(logMutex);
        
        // If no log file path is set, log to console
        if (this->enableConsoleLogging) {
            printf("%s\n", formatLog(level, message, location, this->enableColor, this->enableTimestamping).c_str());
        }

        // If log file path is set, log to file
        if (!this->LogFilePath.empty() || !OneTimeLogFilePath.empty()) {
            FILE* file = fopen(OneTimeLogFilePath.empty() ? this->LogFilePath.c_str() : OneTimeLogFilePath.c_str(), "a");
            if (file) {
                // Format the file without ANSI color codes
                fprintf(file, "%s\n", formatLog(level, message, location, false, this->enableTimestamping).c_str());
                fclose(file);
            }
        }
}

// Example: [main.cpp:6] [INFO] This is an informational message.
std::string Sapling::formatLog(LogLevel level, const std::string &message,
    const std::source_location location = std::source_location::current(), 
    bool ANSIIColor = true, bool timestamp = true) {

    std::string currentTime = timestamp ? "[" + getCurrentTime() + "] " : "";
    std::string fileName = std::filesystem::path(location.file_name()).filename().string();
    std::string color = ANSIIColor ? LogLevelColors[level] : "";
    std::string ANSIIreset = ANSIIColor ? "\033[0m" : "";

    return std::format("{}[{}:{}] {}[{}] {} {}",
        currentTime,
        fileName,
        location.line(),
        color,
        LogLevelNames[level],
        message,
        ANSIIreset);
}

// Small macros for easier logging
#define LOG_INFO(sapling, msg) sapling.log(msg, INFO)
#define LOG_ERR(sapling, msg)  sapling.log(msg, ERROR)
#define LOG_WARN(sapling, msg) sapling.log(msg, WARNING)
#define LOG_DEBUG(sapling, msg) sapling.log(msg, DEBUG)