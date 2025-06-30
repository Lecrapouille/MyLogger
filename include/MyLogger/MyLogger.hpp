#pragma once

#include "MyLogger/Strategies/LogLineFormatter.hpp"
#include "MyLogger/Strategies/LogWriter.hpp"

#include <memory>
#include <mutex>

// Forward declarations
class Trace;
enum class LogLevel;

// *****************************************************************************
//! \brief Thread-safe template-based Logger class.
//! \tparam WriterType The type of the writer (i.e. FileLogWriter,
//!   ConsoleLogWriter, SocketLogWriter).
//! \tparam FileFormatterType The type of the file formatter (i.e.
//!   OpenTelemetryFileFormatter).
//! \tparam LineFormatterType The type of the line formatter (i.e.
//!   OpenTelemetryLineFormatter).
// *****************************************************************************
template <typename WriterType,
          typename FileFormatterType,
          typename LineFormatterType>
class Logger
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor. Delegates to the writer and file formatter.
    //! \param p_writer The writer to use.
    //! \param p_line_formatter The line formatter to use.
    //! \param p_file_formatter The file formatter to use (contains line
    //! formatter).
    //-------------------------------------------------------------------------
    explicit Logger(std::unique_ptr<WriterType> p_writer,
                    std::unique_ptr<LineFormatterType> p_line_formatter,
                    std::unique_ptr<FileFormatterType> p_file_formatter)
        : m_writer(std::move(p_writer)),
          m_line_formatter(std::move(p_line_formatter)),
          m_file_formatter(std::move(p_file_formatter))
    {
        std::lock_guard<std::mutex> lock(m_log_mutex);
        m_writer->writeHeader(*m_file_formatter);
    }

    //-------------------------------------------------------------------------
    //! \brief Destructor.
    //-------------------------------------------------------------------------
    ~Logger()
    {
        std::lock_guard<std::mutex> lock(m_log_mutex);
        m_writer->writeFooter(*m_file_formatter);
    }

    //-------------------------------------------------------------------------
    //! \brief Log a message with level and trace.
    //! \param p_level The log level.
    //! \param p_trace The trace containing log data.
    //-------------------------------------------------------------------------
    void log(LogLevel p_level, const Trace& p_trace)
    {
        std::lock_guard<std::mutex> lock(m_log_mutex);
        m_writer->writeLine(p_level, p_trace);
    }

    //-------------------------------------------------------------------------
    //! \brief Log a message.
    //! \param p_level The log level.
    //! \param p_message The message to log.
    //-------------------------------------------------------------------------
    void log(LogLevel p_level, const std::string& p_message)
    {
        std::lock_guard<std::mutex> lock(m_log_mutex);
        m_writer->writeLine(p_level, p_message);
    }

    //-------------------------------------------------------------------------
    //! \brief Flush the logger.
    //-------------------------------------------------------------------------
    void flush()
    {
        std::lock_guard<std::mutex> lock(m_log_mutex);
        m_writer->flush();
    }

    //-------------------------------------------------------------------------
    //! \brief Get a reference to the writer.
    //-------------------------------------------------------------------------
    WriterType& getWriter()
    {
        return *m_writer;
    }
    const WriterType& getWriter() const
    {
        return *m_writer;
    }

private:

    //! \brief The writer.
    std::unique_ptr<WriterType> m_writer;
    //! \brief The line formatter.
    std::unique_ptr<LineFormatterType> m_line_formatter;
    //! \brief The file formatter.
    std::unique_ptr<FileFormatterType> m_file_formatter;
    //! \brief The log mutex.
    std::mutex m_log_mutex;
};