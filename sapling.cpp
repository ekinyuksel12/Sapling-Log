#include <format>
#include <filesystem>
#include <fstream>

#include "sapling.h"

// Helper functions
// Function to strip ANSI color codes from a string
std::string stripAnsiCodes(const std::string& input) {
    std::string output = input;
    const std::string start_sequence = "\033[";

    size_t pos = 0;
    while ((pos = output.find(start_sequence, pos)) != std::string::npos) {
        // Find the ending 'm' character
        size_t end_pos = output.find('m', pos);

        if (end_pos != std::string::npos) {
            // Erase the sequence, including the starting '\033[' and the ending 'm'
            output.erase(pos, end_pos - pos + 1);
        } else {
            // If the sequence is malformed (no ending 'm'), skip past the start and continue search
            pos += start_sequence.length();
        }
    }
    return output;
}

// Function to get current time as string
std::string getCurrentTime() {
    auto const time = std::chrono::current_zone()->to_local(std::chrono::system_clock::now());
    return std::format("{:%Y-%m-%d %X}", time);
}



// Constructor with default parameters
Sapling::Sapling(SaplingConfig config) : m_config(config) {
    if (!m_config.logFilePath.empty()) {
        m_logFileStream.open(m_config.logFilePath, std::ios::app);
    }
}

// Destructor to close file stream if open
Sapling::~Sapling() {
    if (m_logFileStream.is_open()) {
        m_logFileStream.close();
    }
}

void Sapling::log(const std::string &message, LogLevel level, 
        const std::source_location location) {

        // Thread-safe logging using mutex
        std::lock_guard<std::mutex> lock(m_logMutex);

        std::string logMessage = formatLog(level, message, location);

        // Log to console if enabled
        if (m_config.enableConsole) {
            // Create the log message
            std::cout << logMessage << std::endl;
        }

        // Log to file if path is set
        if (m_logFileStream.is_open()) {
            m_logFileStream << stripAnsiCodes(logMessage) << "\n";
        }
}

void Sapling::updateConfig(const SaplingConfig& newConfig) {
    // Use mutex for thread safety during reconfiguration
    std::lock_guard<std::mutex> lock(m_logMutex);

    // Check if the log file path has changed. If so, handle file stream accordingly.
    if (m_config.logFilePath != newConfig.logFilePath) {
        // Close the old stream, if it was open
        if (m_logFileStream.is_open()) {
            m_logFileStream.close();
        }

        // Update the configuration
        m_config = newConfig;

        // Open the new stream, if a new path is provided
        if (!m_config.logFilePath.empty()) {
            m_logFileStream.open(m_config.logFilePath, std::ios::app);
            // Error handling: Check if the new file opened successfully
            if (!m_logFileStream.is_open()) {
                // Log an error to console or throw, as file logging will fail
                std::cerr << "[Error] Sapling failed to open new log file: " << m_config.logFilePath << std::endl;
            }
        }
    } else {
        // If the path didn't change, just update the rest of the config settings
        m_config = newConfig;
    }
}

// Example: [main.cpp:6] [INFO] This is an informational message.
std::string Sapling::formatLog(LogLevel level, const std::string &message, const std::source_location location) {
    std::string currentTime = m_config.enableTimestamp ? "[" + getCurrentTime() + "] " : "";

    std::string fileName = std::filesystem::path(location.file_name()).filename().string();

    // Setup color codes if enabled
    std::string color = m_config.enableColor ? LogLevelColors[level] : "";
    std::string ANSIIreset = m_config.enableColor ? "\033[0m" : "";

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