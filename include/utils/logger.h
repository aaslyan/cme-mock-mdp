#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>

namespace cme_mock {

enum class LogLevel {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4
};

class Logger {
public:
    static Logger& instance() {
        static Logger instance;
        return instance;
    }
    
    void set_level(LogLevel level) { min_level_ = level; }
    void set_file(const std::string& filename);
    
    void log(LogLevel level, const std::string& message);
    
    // Convenience methods
    void debug(const std::string& msg) { log(LogLevel::DEBUG, msg); }
    void info(const std::string& msg) { log(LogLevel::INFO, msg); }
    void warning(const std::string& msg) { log(LogLevel::WARNING, msg); }
    void error(const std::string& msg) { log(LogLevel::ERROR, msg); }
    void critical(const std::string& msg) { log(LogLevel::CRITICAL, msg); }
    
private:
    Logger() = default;
    
    LogLevel min_level_ = LogLevel::INFO;
    std::ofstream file_;
    
    std::string level_to_string(LogLevel level);
    std::string get_timestamp();
};

// Macros for convenient logging
#define LOG_DEBUG(msg) cme_mock::Logger::instance().debug(msg)
#define LOG_INFO(msg) cme_mock::Logger::instance().info(msg)
#define LOG_WARNING(msg) cme_mock::Logger::instance().warning(msg)
#define LOG_ERROR(msg) cme_mock::Logger::instance().error(msg)
#define LOG_CRITICAL(msg) cme_mock::Logger::instance().critical(msg)

} // namespace cme_mock