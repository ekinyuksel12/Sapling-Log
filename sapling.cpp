#include "sapling.h"
#include <format>

Sapling::Sapling(std::string LogFilePath, bool enableConsoleLogging, bool enableColor) {
    this->LogFilePath = LogFilePath;
    this->enableConsoleLogging = enableConsoleLogging;
    this->enableColor = enableColor;
}

void Sapling::log(LogLevel level, const std::string &message, std::string OneTimeLogFilePath,
    const std::source_location location) {
        // If no log file path is set, log to console
        if (this->enableConsoleLogging) {
            printf("%s\n", formatLog(level, message, location, this->enableColor).c_str());
        }

        // If log file path is set, log to file
        else if (!this->LogFilePath.empty() || !OneTimeLogFilePath.empty()) {
            FILE* file = fopen(OneTimeLogFilePath.empty() ? this->LogFilePath.c_str() : OneTimeLogFilePath.c_str(), "a");
            if (file) {
                // Format the file without ANSI color codes
                fprintf(file, "%s\n", formatLog(level, message, location, false).c_str());
                fclose(file);
            }
        }
}

// Example: [main.cpp:6] [INFO] This is an informational message.
std::string Sapling::formatLog(LogLevel level, const std::string &message,
    const std::source_location location = std::source_location::current(), bool ANSIIColor = true) {
    
    if (ANSIIColor)
    return std::format("[{}:{}] {}[{}] {} \033[0m", 
        get_filename(location.file_name()),
        location.line(),
        LogLevelColors[level],
        LogLevelNames[level],
        message);

    return std::format("[{}:{}] [{}] {}", 
        get_filename(location.file_name()),
        location.line(),
        LogLevelNames[level],
        message);
}