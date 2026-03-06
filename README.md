<p align="center">
  <img src="icon.jpg" alt="Sapling Logo" width="300"/>
</p>

<h1 align="center">Sapling Logger 🌱</h1>

<p align="center">
  <strong>A Modern, Lightweight, Header-Only C++20 Logging Library</strong>
</p>

<p align="center">
  <img src="https://img.shields.io/badge/standard-C%2B%2B20-blue.svg" alt="C++20"/>
  <img src="https://img.shields.io/badge/header--only-yes-brightgreen.svg" alt="Header Only"/>
  <img src="https://img.shields.io/badge/license-MIT-orange.svg" alt="License"/>
  <img src="https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg" alt="Platform"/>
</p>

---

## Overview

**Sapling** is a zero-dependency, header-only logging library for C++20. Drop a single file into your project and get color-coded console output, automatic file logging with rotation, thread-safe writes, and source-location tracking — all out of the box.

In distributed systems and high-frequency environments, logging I/O is often a massive bottleneck. Sapling-Log was engineered to solve this by providing a lightweight, lock-optimized logging pipeline. 
* **Zero-Dependency Architecture:** Eliminates supply chain risks and bloat, making it ideal for embedded systems and strict enterprise environments.
* **Concurrency Optimized:** Thread-safe execution using modern C++20 synchronization primitives, ensuring telemetry is accurately captured without race conditions.
* **Automated Data Lifecycle:** Built-in log rotation (size/time-based) prevents storage saturation and server crashes in production environments.

---

## Features

| Feature | Description |
|---|---|
| **Header-Only** | Single file — just include `sapling.hpp`. No linking, no build system changes. |
| **ANSI Color Output** | Color-coded log levels in the console: Debug (white), Info (green), Warning (yellow), Error (red). |
| **File Logging** | Automatic log file creation with clean output (ANSI codes stripped). |
| **Log Rotation** | Size-based and time-based file rotation with configurable thresholds. |
| **Thread-Safe** | All log writes are mutex-protected for safe concurrent use. |
| **Source Location** | Each log message automatically captures file name and line number (C++20 `source_location`). |
| **Configurable** | Fine-grained control via `SaplingConfig` — toggle console, file, timestamps, colors, log levels, and more. |
| **Microsecond Precision** | Optional microsecond timestamps for high-resolution logging. |

---

## Installation

Copy `sapling.hpp` into your project's include directory. That's it.

```
your_project/
├── include/
│   └── sapling.hpp
└── main.cpp
```

---

## Quick Start

```cpp
#include "sapling.hpp"

int main() {
    Sapling logger;

    logger.log("System initialized", INFO);
    logger.log("Variable x is 42", DEBUG);
    logger.log("Memory usage is high", WARNING);
    logger.log("Connection failed!", ERROR);

    return 0;
}
```

---

## Configuration

All behavior is controlled through the `SaplingConfig` struct:

```cpp
SaplingConfig config;
config.logName           = "myapp";          // Base name for log files
config.logFileDirectory  = "./logs";         // Directory for log files
config.logFileExtension  = "log";            // File extension
config.enableFileLogging = true;             // Write to file
config.enableConsole     = true;             // Write to console
config.enableColor       = true;             // ANSI colors in console
config.enableTimestamp   = true;             // Timestamps in log messages
config.showMicroseconds  = false;            // Microsecond precision
config.consoleLogLevel   = DEBUG;            // Minimum level for console
config.fileLogLevel      = INFO;             // Minimum level for file

Sapling logger(config);
```

### Log Level Filtering

Log levels follow this severity order: `DEBUG` < `INFO` < `WARNING` < `ERROR`

Set `consoleLogLevel` or `fileLogLevel` to filter out messages below a certain severity.

### File Rotation

```cpp
config.enableFileRotation = true;
config.maxFileSizeKB      = 1024;                        // Rotate after 1 MB
config.rotationInterval   = std::chrono::seconds(3600);  // Rotate every hour
```

When rotation triggers, the current file is closed and a new timestamped file is created automatically.

---

## Log Macros

Shorthand macros are available for convenience:

```cpp
LOG_INFO("Application started");
LOG_DEBUG("Value of x = 10");
LOG_WARN("Disk space running low");
LOG_ERR("Failed to open socket");
```

---

## Requirements

- **C++20** compatible compiler
  - GCC 11+
  - Clang 13+
  - MSVC 2019 (16.10+)

---

<p align="center">
  <sub>Built with 🌱 by <a href="https://github.com/ekinyuksel12">ekinyuksel12</a></sub>
</p>
