#include <iostream>
#include <source_location>

enum LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

static const char* LogLevelNames[] = { "DEBUG", "INFO", "WARNING", "ERROR" };
static const char* LogLevelColors[] = { "\033[0;37m", "\033[0;32m", "\033[0;33m", "\033[0;31m" };

class Sapling {
private:
    
public:
    void log(LogLevel level, const std::string &message,
        const std::source_location location = std::source_location::current());

    constexpr const char* get_filename(const char* path) {
    const char* file = path;
    while (*path) {
        if (*path == '/' || *path == '\\') {
            file = path + 1;
        }
        path++;
    }
    return file;
}
};