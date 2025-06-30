#pragma once

#include <mutex>
#include <string>

// Forward declarations
class Trace;
enum class LogLevel;

// *****************************************************************************
//! \brief Thread-safe template-based strategy pattern for writing logs.
//! This uses CRTP (Curiously Recurring Template Pattern) to avoid virtual
//! calls.
//! \tparam Derived The derived class.
//! \tparam LineFormatterType The type of the line formatter.
// *****************************************************************************
template <typename Derived, typename LineFormatterType>
class LogWriter
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor that takes a reference to the line formatter.
    //! \param p_line_formatter Reference to the line formatter to use.
    //-------------------------------------------------------------------------
    explicit LogWriter(LineFormatterType& p_line_formatter)
        : m_line_formatter(p_line_formatter)
    {
    }

    //-------------------------------------------------------------------------
    //! \brief Write a formatted log message by delegating to formatter.
    //! Uses a single lock for the entire write operation for thread safety.
    //! \param p_level The log level.
    //! \param p_trace The trace containing log data.
    //-------------------------------------------------------------------------
    void writeLine(LogLevel p_level, const Trace& p_trace)
    {
        std::string begin = m_line_formatter.formatBegin(p_level);
        std::string middle = m_line_formatter.formatMiddle(p_trace);
        std::string end = m_line_formatter.formatEnd();

        // Write each part under the same lock
        std::lock_guard<std::mutex> lock(m_write_mutex);
        static_cast<Derived*>(this)->writeImpl(begin);
        static_cast<Derived*>(this)->writeImpl(middle);
        static_cast<Derived*>(this)->writeImpl(end);
    }

    //-------------------------------------------------------------------------
    //! \brief Write a pre-formatted message (fallback method).
    //! \param p_level The log level.
    //! \param p_message The formatted message to write.
    //-------------------------------------------------------------------------
    void writeLine(LogLevel p_level, const std::string& p_message)
    {
        std::string begin = m_line_formatter.formatBegin(p_level);
        std::string end = m_line_formatter.formatEnd();

        std::lock_guard<std::mutex> lock(m_write_mutex);
        static_cast<Derived*>(this)->writeImpl(begin);
        static_cast<Derived*>(this)->writeImpl(p_message);
        static_cast<Derived*>(this)->writeImpl(end);
    }

    //-------------------------------------------------------------------------
    //! \brief Flush the writer.
    //-------------------------------------------------------------------------
    void flush()
    {
        std::lock_guard<std::mutex> lock(m_write_mutex);
        static_cast<Derived*>(this)->flushImpl();
    }

    //-------------------------------------------------------------------------
    //! \brief Write header using file formatter (if provided).
    //! Template parameter allows both file and line formatters.
    //-------------------------------------------------------------------------
    template <typename FileFormatterType>
    void writeHeader(FileFormatterType& p_file_formatter)
    {
        std::string header = p_file_formatter.header();
        if (!header.empty())
        {
            std::lock_guard<std::mutex> lock(m_write_mutex);
            static_cast<Derived*>(this)->writeImpl(header);
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Write footer using file formatter (if provided).
    //! Template parameter allows both file and line formatters.
    //-------------------------------------------------------------------------
    template <typename FileFormatterType>
    void writeFooter(FileFormatterType& p_file_formatter)
    {
        std::string footer = p_file_formatter.footer();
        if (!footer.empty())
        {
            std::lock_guard<std::mutex> lock(m_write_mutex);
            static_cast<Derived*>(this)->writeImpl(footer);
        }
    }

private:

    //! \brief The derived line formatter.
    LineFormatterType& m_line_formatter;
    //! \brief Protects all write operations.
    mutable std::mutex m_write_mutex;
};