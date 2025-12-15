#include <source_location>
#include <iostream>
#include <string>
#include <chrono>
#include <format>
#include <mutex>
#include <fstream>

enum LogLevel{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

inline constexpr const char* LogLevelNames[] = { "DEBUG", "INFO", "WARNING", "ERROR" };
inline constexpr const char* LogLevelColors[] = { "\033[0;37m", "\033[0;32m", "\033[0;33m", "\033[0;31m" };

struct SaplingConfig {
    std::string logFilePath = "";
    bool enableConsole = true;
    bool enableColor = true;
    bool enableTimestamp = true;
};

class Sapling {
private:
    SaplingConfig m_config;
    std::mutex m_logMutex;
    std::ofstream m_logFileStream;

    std::string formatLog(LogLevel level, const std::string &message,
        const std::source_location location);

public:
    // Constructor with default config
    Sapling(SaplingConfig config = SaplingConfig());

    // Destructor to close file stream if open
    ~Sapling();

    void updateConfig(const SaplingConfig& newConfig);

    void log(const std::string &message, LogLevel level = INFO,
        const std::source_location location = std::source_location::current());
};