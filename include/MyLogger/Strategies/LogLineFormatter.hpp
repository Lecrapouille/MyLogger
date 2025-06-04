#pragma once

#include <string>

// Forward declarations
class Trace;
enum class LogLevel;

// *****************************************************************************
//! \brief Template-based strategy pattern for formatting log lines.
//! This is a CRTP (Curiously Recurring Template Pattern) base class.
//! Focuses only on line formatting, not file-level formatting.
// *****************************************************************************
template<typename Derived>
class LogLineFormatter
{
public:

    //-------------------------------------------------------------------------
    //! \brief Format the beginning of a log line.
    //-------------------------------------------------------------------------
    std::string formatBegin(LogLevel p_level)
    {
        return static_cast<Derived*>(this)->formatBeginImpl(p_level);
    }

    //-------------------------------------------------------------------------
    //! \brief Format the middle part of a log line with trace data.
    //-------------------------------------------------------------------------
    std::string formatMiddle(const Trace& p_trace)
    {
        return static_cast<Derived*>(this)->formatMiddleImpl(p_trace);
    }

    //-------------------------------------------------------------------------
    //! \brief Format the end of a log line.
    //-------------------------------------------------------------------------
    std::string formatEnd()
    {
        return static_cast<Derived*>(this)->formatEndImpl();
    }

    //-------------------------------------------------------------------------
    //! \brief Format a complete line (convenience method).
    //-------------------------------------------------------------------------
    std::string line(LogLevel p_level, const Trace& p_trace)
    {
        return formatBegin(p_level) + formatMiddle(p_trace) + formatEnd();
    }

protected:
    // Default implementations that derived classes can override
    std::string formatEndImpl() { return "\n"; }
};