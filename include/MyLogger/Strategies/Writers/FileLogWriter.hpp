#pragma once

#include "MyLogger/Strategies/LogWriter.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

// *****************************************************************************
//! \brief Template-based File writer for logging with improved thread safety.
// *****************************************************************************
template<typename LineFormatterType>
class FileLogWriter : public LogWriter<FileLogWriter<LineFormatterType>, LineFormatterType>
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor.
    //-------------------------------------------------------------------------
    explicit FileLogWriter(const std::string& p_filename, LineFormatterType& p_line_formatter)
        : LogWriter<FileLogWriter<LineFormatterType>, LineFormatterType>(p_line_formatter)
        , m_file(p_filename, std::ios::app)
    {
        if (!m_file.is_open())
        {
            throw std::runtime_error("Cannot open log file: " + p_filename);
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Write the beginning part of a log message.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void writeBeginImpl(const std::string& p_begin)
    {
        m_file << p_begin;
    }

    //-------------------------------------------------------------------------
    //! \brief Write the middle part of a log message.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void writeMiddleImpl(const std::string& p_middle)
    {
        m_file << p_middle;
    }

    //-------------------------------------------------------------------------
    //! \brief Write the end part of a log message.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void writeEndImpl(const std::string& p_end)
    {
        m_file << p_end;
        m_file.flush(); // Ensure data is written to disk
    }

    //-------------------------------------------------------------------------
    //! \brief Write a complete formatted message (fallback method).
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void writeImpl(const std::string& p_message)
    {
        m_file << p_message;
        m_file.flush();
    }

    //-------------------------------------------------------------------------
    //! \brief Flush the file output.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void flushImpl()
    {
        m_file.flush();
    }

private:
    std::ofstream m_file;
};