#include <iostream>

constexpr const char* file_name(const char* path) {
    const char* file = path;
    while (*path) {
        if (*path == '/' || *path == '\\') {
            file = path + 1;
        }
        path++;
    }
    return file;
}

enum LogLevel
{
    DEBUG,
    INFO,
    WARNING,
    ERROR
};

static const char* LogLevelNames[] = { "DEBUG", "INFO", "WARNING", "ERROR" };

class sapling {
private:
    
public:
    sapling();
    void log(LogLevel level, const char* file, int line, const std::string &message);
};

sapling::sapling()
{
    std::cout << "sapling constructor called" << std::endl;
}

void sapling::log(LogLevel level, const char* file, int line, const std::string &message)
{
    printf("[%s:%d] [%s] %s\n", file, line, LogLevelNames[level], message.c_str());
}

#define logInfo(message) log(INFO, file_name(__FILE__), __LINE__, message)
#define logDebug(message) log(DEBUG, file_name(__FILE__), __LINE__, message)
#define logWarning(message) log(WARNING, file_name(__FILE__), __LINE__, message)
#define logError(message) log(ERROR, file_name(__FILE__), __LINE__, message)