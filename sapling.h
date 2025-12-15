#include <source_location>z
#include <iostream>
#include <string>
#include <chrono>
#include <format>

enum LogLevel{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

inline constexpr const char* LogLevelNames[] = { "DEBUG", "INFO", "WARNING", "ERROR" };
inline constexpr const char* LogLevelColors[] = { "\033[0;37m", "\033[0;32m", "\033[0;33m", "\033[0;31m" };

class Sapling {
private:
    std::string getCurrentTime() {
        auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
        return std::format("{:%Y-%m-%d %X}", time);
    }

    std::string LogFilePath;
    bool enableConsoleLogging;
    bool enableColor;
    bool enableTimestamping;

    std::string formatLog(LogLevel level, const std::string &message,
        const std::source_location location, 
        bool ANSIIColor, bool timestamp);

public:
    // Constructor with default parameters
    Sapling(std::string LogFilePath = "", bool enableConsoleLogging = true,
         bool enableColor = true, bool enableTimestamping = true);

    // Setter for log file path
    void setLogFilePath(std::string LogFilePath) {
        this->LogFilePath = LogFilePath;
    }

    void setConsoleLogging(bool enable) {
        this->enableConsoleLogging = enable;
    }

    void setColor(bool enable) {
        this->enableColor = enable;
    }

    void setTimestamping(bool enable) {
        this->enableTimestamping = enable;
    }

    void log(const std::string &message, LogLevel level = INFO, std::string OneTimeLogFilePath = "",
        const std::source_location location = std::source_location::current());
};