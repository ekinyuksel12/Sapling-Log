# Sapling Logger 🌱
**A Modern, Lightweight, Header-Only C++20 Logger**

Sapling is a simple logging library designed for modern C++.

![C++20](https://img.shields.io/badge/standard-C%2B%2B20-blue.svg)

## Features
* **Header-Only:** Drop `sapling.hpp` into your project and go. No linking required.
* **Color Coded:** ANSI color support for console output (Debug, Info, Warning, Error).
* **Flexible Config:** Toggle console logging, file logging, timestamps, and colors via a simple struct.
* **Clean File Output:** Automatically strips ANSI color codes when writing to log files.

## Installation
Just copy `sapling.hpp` into your project's include directory.

## Usage

### Basic Example
```cpp
#include "sapling.hpp"

int main() {
    // Initialize with defaults (Console logging ON, Timestamp ON)
    Sapling logger;

    logger.log("System initialized", INFO);
    logger.log("Variable x is 42", DEBUG);
    
    // Using macros for shorthand
    LOG_WARN(logger, "Memory usage is high");
    LOG_ERR(logger, "Connection failed!");

    return 0;
}

Advanced Configuration (File Logging)
C++

#include "sapling.hpp"

int main() {
    // Configure settings
    SaplingConfig config;
    config.logFilePath = "app.log";
    config.enableConsole = true;
    config.enableTimestamp = true;

    Sapling logger(config);

    logger.log("This goes to both console and file");
    
    // Runtime reconfiguration
    config.logFilePath = "new_log.txt";
    logger.updateConfig(config); // Automatically closes old file, opens new one
}
```

## Requirements

    C++20 compatible compiler (GCC 11+, Clang 13+, MSVC 2019 16.10+)