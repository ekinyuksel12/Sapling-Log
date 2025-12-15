#include "sapling.h"

void Sapling::log(LogLevel level, const std::string &message, 
    const std::source_location location)
{
    printf("[%s:%d] [%s] %s\n", location.file_name(), location.line(), LogLevelNames[level], message.c_str());
}