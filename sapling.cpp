#include "sapling.h"

void Sapling::log(LogLevel level, const std::string &message,
    const std::source_location location)
{
    printf("[%s:%d] %s[%s] %s \033[0m\n", get_filename(location.file_name()),
        location.line(), LogLevelColors[level], LogLevelNames[level],
        message.c_str());
}