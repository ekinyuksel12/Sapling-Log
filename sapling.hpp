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
    std::string logFilePath = "";
    bool enableConsole = true;
    bool enableColor = true;
    bool enableTimestamp = true;

    // File Rotation Settings
    bool enableFileRotation = false;
    size_t maxFileSizeKB = 5120; // Default is 5 MB
};

// --- Main Class ---

class Sapling {
private:
    SaplingConfig m_config;
    std::mutex m_logMutex;
    std::ofstream m_logFileStream;

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

    // Helper: Get formatted time
    std::string getCurrentTimestamp() {
        auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
        return std::format("{:%Y-%m-%d %X}", time);
    }

    // Helper: Get file-safe timestamp
    std::string getFileSafeTimestamp() {
        auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
        return std::format("{:%Y-%m-%d_%H-%M-%S}", time);
    }

    // Helper: 
    void rotateFileIfNeeded() {
        // Check if rotation is enabled and file is open
        if (!m_config.enableFileRotation || !m_logFileStream.is_open()) return;

        // Check current file size
        long currentSize = m_logFileStream.tellp(); // We use tellp to get the size
        if (currentSize < 0) return; // Unable to get size

        if (currentSize >= (long)m_config.maxFileSizeKB * 1024) {
            m_logFileStream.close();

            std::string newFileName = m_config.logFilePath + "." + getFileSafeTimestamp();
            std::filesystem::rename(m_config.logFilePath, newFileName);

            m_logFileStream.open(m_config.logFilePath, std::ios::app);
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
        if (!m_config.logFilePath.empty()) {
            m_logFileStream.open(m_config.logFilePath, std::ios::app);
        }
    }

    // Destructor
    ~Sapling() {
        if (m_logFileStream.is_open()) {
            m_logFileStream.close();
        }
    }

    // Configuration Updater
    void updateConfig(const SaplingConfig& newConfig) {
        std::lock_guard<std::mutex> lock(m_logMutex);
        if (m_config.logFilePath != newConfig.logFilePath) {
            if (m_logFileStream.is_open()) m_logFileStream.close();
            
            m_config = newConfig; // Apply new config
            
            if (!m_config.logFilePath.empty()) {
                m_logFileStream.open(m_config.logFilePath, std::ios::app);
                if (!m_logFileStream.is_open()) {
                    std::cerr << "[Error] Sapling failed to open new log file: " << m_config.logFilePath << std::endl;
                }
            }
        } else {
            m_config = newConfig;
        }
    }

    // Primary Log Function
    void log(const std::string &message, LogLevel level = INFO,
             const std::source_location location = std::source_location::current()) {
        std::lock_guard<std::mutex> lock(m_logMutex);
        
        std::string logMessage = formatLog(level, message, location);

        if (m_config.enableConsole) {
            std::cout << logMessage << std::endl;
        }

        if (m_logFileStream.is_open()) {
            rotateFileIfNeeded();

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