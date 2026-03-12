#pragma once
#ifndef VAS_LOGGER_H
#define VAS_LOGGER_H

/**
 * @file logger.h
 * @brief Lightweight, thread-safe logging utility for VAS.
 *
 * Usage:
 *   VAS_LOG_INFO("Application started");
 *   VAS_LOG_WARN("Font not found at path: {}", path);
 *   VAS_LOG_ERROR("Failed to initialize renderer");
 */

#include <string>
#include <fstream>
#include <sstream>
#include <mutex>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <functional>

namespace vas {

 
// Log level
 
enum class LogLevel { DEBUG = 0, INFO, WARN, ERROR };

 
// Logger — singleton
 
class Logger {
public:
    static Logger& instance() {
        static Logger logger;
        return logger;
    }

    /// Set minimum level to actually emit (default: INFO).
    void setLevel(LogLevel level) { m_level = level; }

    /// Optionally mirror output to a file.
    void setOutputFile(const std::string& path) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_file.open(path, std::ios::out | std::ios::app);
    }

    /// Core log method.
    void log(LogLevel level, const std::string& message,
             const char* file = nullptr, int line = 0)
    {
        if (level < m_level) return;

        std::lock_guard<std::mutex> lock(m_mutex);
        const std::string entry = buildEntry(level, message, file, line);

        std::cout << entry << "\n";
        if (m_file.is_open()) m_file << entry << "\n";
    }

    // Non-copyable
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() : m_level(LogLevel::INFO) {}

    static std::string levelTag(LogLevel l) {
        switch (l) {
            case LogLevel::DEBUG: return "\033[37m[DEBUG]\033[0m";
            case LogLevel::INFO:  return "\033[32m[INFO ]\033[0m";
            case LogLevel::WARN:  return "\033[33m[WARN ]\033[0m";
            case LogLevel::ERROR: return "\033[31m[ERROR]\033[0m";
        }
        return "[?????]";
    }

    static std::string timestamp() {
        auto now  = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream ss;
        ss << std::put_time(std::localtime(&time), "%H:%M:%S");
        return ss.str();
    }

    static std::string buildEntry(LogLevel level, const std::string& msg,
                                  const char* file, int line)
    {
        std::ostringstream ss;
        ss << timestamp() << " " << levelTag(level) << " " << msg;
        if (file) ss << "  [" << file << ":" << line << "]";
        return ss.str();
    }

    LogLevel   m_level;
    std::mutex m_mutex;
    std::ofstream m_file;
};

} // namespace vas

 
// Convenience macros
 
#define VAS_LOG_DEBUG(msg) ::vas::Logger::instance().log(::vas::LogLevel::DEBUG, msg, __FILE__, __LINE__)
#define VAS_LOG_INFO(msg)  ::vas::Logger::instance().log(::vas::LogLevel::INFO,  msg, __FILE__, __LINE__)
#define VAS_LOG_WARN(msg)  ::vas::Logger::instance().log(::vas::LogLevel::WARN,  msg, __FILE__, __LINE__)
#define VAS_LOG_ERROR(msg) ::vas::Logger::instance().log(::vas::LogLevel::ERROR, msg, __FILE__, __LINE__)

#endif // VAS_LOGGER_H
