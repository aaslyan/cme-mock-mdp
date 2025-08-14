#include "utils/logger.h"
#include <iomanip>
#include <ctime>

namespace cme_mock {

void Logger::set_file(const std::string& filename) {
    if (file_.is_open()) {
        file_.close();
    }
    file_.open(filename, std::ios::app);
}

void Logger::log(LogLevel level, const std::string& message) {
    if (level < min_level_) {
        return;
    }
    
    std::string log_line = get_timestamp() + " [" + level_to_string(level) + "] " + message;
    
    // Always log to console
    if (level >= LogLevel::ERROR) {
        std::cerr << log_line << std::endl;
    } else {
        std::cout << log_line << std::endl;
    }
    
    // Also log to file if configured
    if (file_.is_open()) {
        file_ << log_line << std::endl;
        file_.flush();
    }
}

std::string Logger::level_to_string(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:    return "DEBUG";
        case LogLevel::INFO:     return "INFO";
        case LogLevel::WARNING:  return "WARN";
        case LogLevel::ERROR:    return "ERROR";
        case LogLevel::CRITICAL: return "CRIT";
        default:                 return "UNKNOWN";
    }
}

std::string Logger::get_timestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

} // namespace cme_mock