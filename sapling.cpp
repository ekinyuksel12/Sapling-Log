#include "sapling.h"
#include <format>

void Sapling::log(LogLevel level, const std::string &message,
    const std::source_location location) {
        printf("%s\n", formatLog(level, message, location).c_str());
}

// Example: [main.cpp:6] [INFO] This is an informational message.
std::string Sapling::formatLog(LogLevel level, const std::string &message,
    const std::source_location location) {

    return std::format("[{}:{}] {}[{}] {} \033[0m", 
        get_filename(location.file_name()),
        location.line(),
        LogLevelColors[level],
        LogLevelNames[level],
        message);
}