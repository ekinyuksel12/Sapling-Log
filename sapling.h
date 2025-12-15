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

class Sapling {
private:
    
public:
    void log(LogLevel level, const std::string &message,
        const std::source_location location = std::source_location::current());
};