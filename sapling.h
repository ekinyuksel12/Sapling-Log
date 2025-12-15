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

class sapling {
private:
    
public:
    sapling();
    void log(LogLevel level, const std::string &message, const std::source_location location = std::source_location::current());
};

void sapling::log(LogLevel level, const std::string &message, const std::source_location location)
{
    printf("[%s:%d] [%s] %s\n", location.file_name(), location.line(), LogLevelNames[level], message.c_str());
}