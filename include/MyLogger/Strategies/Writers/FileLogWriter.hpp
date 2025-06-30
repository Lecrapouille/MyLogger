#pragma once

#include "MyLogger/Strategies/LogFileFormatter.hpp"
#include "MyLogger/Strategies/LogWriter.hpp"

#include <fstream>
#include <string>

// *****************************************************************************
//! \brief Template-based file writer for logging with improved thread safety.
//! \tparam LineFormatterType The type of the line formatter.
// *****************************************************************************
template <typename LineFormatterType>
class FileLogWriter
    : public LogWriter<FileLogWriter<LineFormatterType>, LineFormatterType>
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor that uses file formatter configuration.
    //! Extracts filename and mode from the file formatter to avoid duplication.
    //! \param p_file_formatter The file formatter containing filename and mode.
    //-------------------------------------------------------------------------
    template <typename FileFormatterType>
    FileLogWriter(FileFormatterType& p_file_formatter)
        : LogWriter<FileLogWriter<LineFormatterType>, LineFormatterType>(
              p_file_formatter.getLineFormatter()),
          m_filename(p_file_formatter.getFilename()),
          m_file_mode(p_file_formatter.getFileMode())
    {
        openFile();
    }

    //-------------------------------------------------------------------------
    //! \brief Legacy constructor for backward compatibility.
    //! \param p_filename The name of the log file.
    //! \param p_line_formatter Reference to the line formatter.
    //! \param p_mode The file mode (defaults to Append).
    //-------------------------------------------------------------------------
    FileLogWriter(const std::string& p_filename,
                  LineFormatterType& p_line_formatter,
                  FileMode p_mode = FileMode::Append)
        : LogWriter<FileLogWriter<LineFormatterType>, LineFormatterType>(
              p_line_formatter),
          m_filename(p_filename),
          m_file_mode(p_mode)
    {
        openFile();
    }

    //-------------------------------------------------------------------------
    //! \brief Destructor.
    //-------------------------------------------------------------------------
    ~FileLogWriter()
    {
        if (m_file.is_open())
        {
            m_file.close();
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Write a complete formatted message.
    //! Called under lock by base class.
    //! \param p_message The message to write.
    //-------------------------------------------------------------------------
    void writeImpl(const std::string& p_message)
    {
        if (m_file.is_open())
        {
            m_file << p_message;
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Flush the file output.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void flushImpl()
    {
        if (m_file.is_open())
        {
            m_file.flush();
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Check if the file is open.
    //-------------------------------------------------------------------------
    bool isOpen() const
    {
        return m_file.is_open();
    }

    //-------------------------------------------------------------------------
    //! \brief Get the current filename.
    //-------------------------------------------------------------------------
    const std::string& getFilename() const
    {
        return m_filename;
    }

private:

    //-------------------------------------------------------------------------
    //! \brief Open the file according to the specified mode.
    //-------------------------------------------------------------------------
    void openFile()
    {
        std::ios_base::openmode mode = std::ios_base::out;

        if (m_file_mode == FileMode::Append)
        {
            mode |= std::ios_base::app;
        }
        else // FileMode::Create
        {
            mode |= std::ios_base::trunc;
        }

        m_file.open(m_filename, mode);
    }

    //! \brief The output file stream
    std::ofstream m_file;
    //! \brief The filename
    std::string m_filename;
    //! \brief The file mode
    FileMode m_file_mode;
};