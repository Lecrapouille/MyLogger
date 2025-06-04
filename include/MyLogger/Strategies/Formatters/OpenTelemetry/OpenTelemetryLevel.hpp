#pragma once

#include <string>

// *****************************************************************************
//! \brief Log level compatible with OpenTelemetry severity levels.
// *****************************************************************************
enum class LogLevel
{
    TRACE = 1,    // OpenTelemetry TRACE (1-4)
    DEBUG = 5,    // OpenTelemetry DEBUG (5-8)
    INFO = 9,     // OpenTelemetry INFO (9-12)
    WARNING = 13, // OpenTelemetry WARN (13-16)
    ERROR = 17,   // OpenTelemetry ERROR (17-20)
    FATAL = 21    // OpenTelemetry FATAL (21-24)
};

// *****************************************************************************
//! \brief Convert a log level to a string.
// *****************************************************************************
inline std::string to_string(LogLevel p_level)
{
    switch (p_level)
    {
        case LogLevel::TRACE:
            return "TRACE";
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARNING:
            return "WARN";
        case LogLevel::ERROR:
            return "ERROR";
        case LogLevel::FATAL:
            return "FATAL";
        default:
            return "UNKNOWN";
    }
}

// *****************************************************************************
//! \brief Convert a log level to OpenTelemetry severity number.
// *****************************************************************************
inline int to_severity_number(LogLevel p_level)
{
    return static_cast<int>(p_level);
}