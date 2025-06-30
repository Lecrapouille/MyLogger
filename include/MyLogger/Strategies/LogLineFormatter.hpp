#pragma once

#include <string>

// Forward declarations
class Trace;
enum class LogLevel;

// *****************************************************************************
//! \brief Template-based strategy pattern for line-level formatting.
//! This uses CRTP (Curiously Recurring Template Pattern) to avoid virtual
//! calls while providing a common interface for line formatters.
//! \tparam Derived The derived class implementing the formatter.
// *****************************************************************************
template <typename Derived>
class LogLineFormatter
{
public:

    //-------------------------------------------------------------------------
    //! \brief Format the beginning of a log line.
    //! \param p_level The log level.
    //! \return The formatted beginning string.
    //-------------------------------------------------------------------------
    std::string formatBegin(LogLevel p_level) const
    {
        return static_cast<const Derived*>(this)->formatBeginImpl(p_level);
    }

    //-------------------------------------------------------------------------
    //! \brief Format the middle part with trace data.
    //! \param p_trace The trace containing log data.
    //! \return The formatted middle string.
    //-------------------------------------------------------------------------
    std::string formatMiddle(const Trace& p_trace) const
    {
        return static_cast<const Derived*>(this)->formatMiddleImpl(p_trace);
    }

    //-------------------------------------------------------------------------
    //! \brief Format the end of a log line.
    //! \return The formatted end string.
    //-------------------------------------------------------------------------
    std::string formatEnd() const
    {
        return static_cast<const Derived*>(this)->formatEndImpl();
    }

    //-------------------------------------------------------------------------
    //! \brief Format a complete line (convenience method).
    //! \param p_level The log level.
    //! \param p_trace The trace containing log data.
    //! \return The formatted line string.
    //-------------------------------------------------------------------------
    std::string line(LogLevel p_level, const Trace& p_trace) const
    {
        return formatBegin(p_level) + formatMiddle(p_trace) + formatEnd();
    }
};