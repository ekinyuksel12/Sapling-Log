#ifndef SAPLING_HPP
#define SAPLING_HPP

#include <source_location>
#include <iostream>
#include <string>
#include <chrono>
#include <format>
#include <mutex>
#include <fstream>
#include <filesystem>

// --- Enums & Constants ---

enum LogLevel {
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

inline constexpr const char* LogLevelNames[] = { "DEBUG", "INFO", "WARNING", "ERROR" };
// ANSI Colors: White, Green, Yellow, Red
inline constexpr const char* LogLevelColors[] = { "\033[0;37m", "\033[0;32m", "\033[0;33m", "\033[0;31m" };

// --- Configuration Struct ---

struct SaplingConfig {
    std::string logName = "sapling";
    std::string logFileDirectory = ".";
    std::string logFileExtension = "log";

    bool enableFileLogging = true;
    bool enableConsole = true;
    bool enableColor = true;
    bool enableTimestamp = true;

    // File Rotation Settings
    bool enableFileRotation = false;
    size_t maxFileSizeKB = 0; // 0KB means file size rotation is disabled
    std::chrono::seconds rotationInterval = std::chrono::seconds(0); // 0 seconds means time-based rotation is disabled

    // Log Filtering Level
    // With the order: DEBUG < INFO < WARNING < ERROR
    LogLevel consoleLogLevel = DEBUG;
    LogLevel fileLogLevel = DEBUG;
};

// --- Main Class ---

class Sapling {
private:
    SaplingConfig m_config;
    std::mutex m_logMutex;
    std::ofstream m_logFileStream;
    std::string m_lastLogPath;

    std::chrono::system_clock::time_point m_nextRotationTime;

    // Helper: Strip ANSI codes for file writing
    std::string stripAnsiCodes(const std::string& input) {
        std::string output = input;
        const std::string start_sequence = "\033[";
        size_t pos = 0;
        while ((pos = output.find(start_sequence, pos)) != std::string::npos) {
            size_t end_pos = output.find('m', pos);
            if (end_pos != std::string::npos) {
                output.erase(pos, end_pos - pos + 1);
            } else {
                pos += start_sequence.length();
            }
        }
        return output;
    }

    // Helper: Get log file name
    std::string generateNewLogFilename() {
        auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
        std::string timeStamp = std::format("{:%Y-%m-%d_%H-%M-%S}", time);

        return m_config.logFileDirectory + m_config.logName + "." + timeStamp + "." + m_config.logFileExtension;
    }

    // Helper: Get formatted time
    std::string getCurrentTimestamp() {
        auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
        return std::format("{:%Y-%m-%d %X}", time);
    }

    void setNextRotationTime() {
        m_nextRotationTime = std::chrono::system_clock::now() + m_config.rotationInterval;
        std::cout << "[Info] Next log rotation scheduled at: " 
                    << std::format("{:%Y-%m-%d %X}", std::chrono::current_zone()->to_local(m_nextRotationTime)) 
                    << std::endl;
    }

    // Helper: Rotate log file if needed
    void rotateFileIfNeeded() {
        // Check if rotation is enabled and file is open
        if (!m_config.enableFileRotation || !m_logFileStream.is_open()) return;

        // Check for time-based rotation
        if (m_config.rotationInterval.count() > 0 && std::chrono::system_clock::now() >= m_nextRotationTime) {
            setNextRotationTime();
            m_logFileStream.close();

            std::string newFileName = generateNewLogFilename();
            std::filesystem::rename(m_lastLogPath, newFileName);

            m_logFileStream.open(m_lastLogPath, std::ios::app);
            m_lastLogPath = newFileName;
        }

        // Check for file size-based rotation
        if (m_config.maxFileSizeKB > 0) {
            m_logFileStream.flush();
            long currentSize = m_logFileStream.tellp(); // We use tellp to get the size
            if (currentSize < 0) return; // Unable to get size

            if (currentSize >= (long)m_config.maxFileSizeKB * 1024) {
                m_logFileStream.close();

                std::string newFileName = generateNewLogFilename();
                std::filesystem::rename(m_lastLogPath, newFileName);

                m_logFileStream.open(m_lastLogPath, std::ios::app);
                m_lastLogPath = newFileName;
            }
        }
    }

    // Internal formatter
    std::string formatLog(LogLevel level, const std::string &message, const std::source_location location) {
        std::string currentTime = m_config.enableTimestamp ? "[" + getCurrentTimestamp() + "] " : "";
        std::string fileName = std::filesystem::path(location.file_name()).filename().string();
        std::string color = m_config.enableColor ? LogLevelColors[level] : "";
        std::string ANSIIreset = m_config.enableColor ? "\033[0m" : "";

        return std::format("{}[{}:{}] {}[{}] {} {}",
            currentTime, fileName, location.line(), color, LogLevelNames[level], message, ANSIIreset);
    }

public:
    // Constructor
    explicit Sapling(SaplingConfig config = SaplingConfig()) : m_config(config) {
        if (m_config.enableFileLogging && !m_config.logFileDirectory.empty()) {
            try {
                if (!std::filesystem::exists(m_config.logFileDirectory)) {
                    std::filesystem::create_directories(m_config.logFileDirectory);
                }
            } catch (const std::filesystem::filesystem_error& e) {
                std::cerr << "[Sapling] Error creating log directory: " << e.what() << std::endl;
                // Disable file logging to prevent further crashes since the directory is invalid
                m_config.enableFileLogging = false; 
            }

            // Ensure the log file directory ends with a separator
            if (m_config.logFileDirectory.back() != '/') m_config.logFileDirectory += "/";

            m_lastLogPath = generateNewLogFilename();

            m_logFileStream.open(m_lastLogPath, std::ios::app);
        }

        // Calculate the next rotation time if interval-based rotation is enabled
        if (m_config.enableFileRotation && m_config.rotationInterval.count() > 0) {
            setNextRotationTime();
        }
    }

    // Destructor
    ~Sapling() {
        if (m_logFileStream.is_open()) {
            m_logFileStream.close();
        }
    }

    // Primary Log Function
    void log(const std::string &message, LogLevel level = INFO,
             const std::source_location location = std::source_location::current()) {
        std::lock_guard<std::mutex> lock(m_logMutex);
        
        std::string logMessage = formatLog(level, message, location);

        if (m_config.enableConsole) {
            if (level < m_config.consoleLogLevel) return; // Skip console log

            std::cout << logMessage << std::endl;
        }

        if (m_logFileStream.is_open()) {
            if (level < m_config.fileLogLevel) return; // Skip file log
            
            rotateFileIfNeeded(); // Check for rotation before writing

            m_logFileStream << stripAnsiCodes(logMessage) << "\n";
        }
    }
};

// --- Macros ---
#define LOG_INFO(msg) log(msg, INFO)
#define LOG_ERR(msg)  log(msg, ERROR)
#define LOG_WARN(msg) log(msg, WARNING)
#define LOG_DEBUG(msg) log(msg, DEBUG)

#endif // SAPLING_HPP