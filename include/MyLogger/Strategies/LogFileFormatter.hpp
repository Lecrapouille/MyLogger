#pragma once

#include <string>

// *****************************************************************************
//! \brief Template-based strategy pattern for file-level formatting.
//! This handles header/footer for log files while delegating line formatting
//! to a LogLineFormatter.
// *****************************************************************************
template<typename Derived, typename LineFormatterType>
class LogFileFormatter
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor that takes a line formatter.
    //-------------------------------------------------------------------------
    explicit LogFileFormatter(LineFormatterType& p_line_formatter)
        : m_line_formatter(p_line_formatter)
    {
    }

    //-------------------------------------------------------------------------
    //! \brief Format the header of the log file.
    //-------------------------------------------------------------------------
    std::string header()
    {
        return static_cast<Derived*>(this)->headerImpl();
    }

    //-------------------------------------------------------------------------
    //! \brief Format the footer of the log file.
    //-------------------------------------------------------------------------
    std::string footer()
    {
        return static_cast<Derived*>(this)->footerImpl();
    }

    //-------------------------------------------------------------------------
    //! \brief Get access to the line formatter.
    //-------------------------------------------------------------------------
    LineFormatterType& getLineFormatter() { return m_line_formatter; }
    const LineFormatterType& getLineFormatter() const { return m_line_formatter; }

protected:
    // Default implementations that derived classes can override
    std::string headerImpl() { return ""; }
    std::string footerImpl() { return ""; }

    LineFormatterType& m_line_formatter;
};