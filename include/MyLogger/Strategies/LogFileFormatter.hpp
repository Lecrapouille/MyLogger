#pragma once

#include <string>

enum class FileMode
{
    Append, //!< Append to existing file or create if doesn't exist
    Create  //!< Create new file or overwrite existing file
};

// *****************************************************************************
//! \brief Template-based strategy pattern for file-level formatting.
//! This handles header/footer for log files while delegating line formatting
//! to a LogLineFormatter.
// *****************************************************************************
template <typename Derived, typename LineFormatterType>
class LogFileFormatter
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor that takes a line formatter, filename and file mode.
    //! \param p_line_formatter The line formatter to use.
    //! \param p_filename The name of the log file.
    //! \param p_mode The file mode (Append or Create).
    //-------------------------------------------------------------------------
    explicit LogFileFormatter(LineFormatterType& p_line_formatter,
                              const std::string& p_filename,
                              const FileMode p_mode)
        : m_line_formatter(p_line_formatter),
          m_filename(p_filename),
          m_file_mode(p_mode)
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
    LineFormatterType& getLineFormatter()
    {
        return m_line_formatter;
    }

    //-------------------------------------------------------------------------
    //! \brief Const get access to the line formatter.
    //-------------------------------------------------------------------------
    const LineFormatterType& getLineFormatter() const
    {
        return m_line_formatter;
    }

    //-------------------------------------------------------------------------
    //! \brief Get the filename.
    //-------------------------------------------------------------------------
    const std::string& getFilename() const
    {
        return m_filename;
    }

    //-------------------------------------------------------------------------
    //! \brief Get the file mode.
    //-------------------------------------------------------------------------
    FileMode getFileMode() const
    {
        return m_file_mode;
    }

protected:

    LineFormatterType& m_line_formatter;
    std::string m_filename;
    FileMode m_file_mode;
};