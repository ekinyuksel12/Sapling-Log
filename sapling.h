#include <iostream>
#include <source_location>
#include <string>

enum LogLevel{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

static const char* LogLevelNames[] = { "DEBUG", "INFO", "WARNING", "ERROR" };
static const char* LogLevelColors[] = { "\033[0;37m", "\033[0;32m", "\033[0;33m", "\033[0;31m" };

class Sapling {
private:
    // Helper function to extract filename from path
    constexpr const char* get_filename(const char* path) {
        const char* file_name = path;
        while (*path) {
            if (*path == '/' || *path == '\\') {
                file_name = path + 1;
            }
            path++;
        }
        return file_name;
    }

    std::string LogFilePath;

    std::string formatLog(LogLevel level, const std::string &message,
        const std::source_location location, 
        bool ANSIIColor);

public:
    Sapling(std::string LogFilePath = "");

    void log(LogLevel level, const std::string &message,
        const std::source_location location = std::source_location::current());
};