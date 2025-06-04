#pragma once

#include "MyLogger/Strategies/LogWriter.hpp"

#include <iostream>
#include <string>

// *****************************************************************************
//! \brief Template-based Console writer for logging with improved thread safety.
// *****************************************************************************
template<typename LineFormatterType>
class ConsoleLogWriter : public LogWriter<ConsoleLogWriter<LineFormatterType>, LineFormatterType>
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor.
    //-------------------------------------------------------------------------
    explicit ConsoleLogWriter(LineFormatterType& p_line_formatter)
        : LogWriter<ConsoleLogWriter<LineFormatterType>, LineFormatterType>(p_line_formatter)
    {
    }

    //-------------------------------------------------------------------------
    //! \brief Write the beginning part of a log message.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void writeBeginImpl(const std::string& p_begin)
    {
        std::cout << p_begin;
    }

    //-------------------------------------------------------------------------
    //! \brief Write the middle part of a log message.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void writeMiddleImpl(const std::string& p_middle)
    {
        std::cout << p_middle;
    }

    //-------------------------------------------------------------------------
    //! \brief Write the end part of a log message.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void writeEndImpl(const std::string& p_end)
    {
        std::cout << p_end;
        std::cout.flush(); // Ensure immediate output
    }

    //-------------------------------------------------------------------------
    //! \brief Write a complete formatted message (fallback method).
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void writeImpl(const std::string& p_message)
    {
        std::cout << p_message;
        std::cout.flush();
    }

    //-------------------------------------------------------------------------
    //! \brief Flush the console output.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void flushImpl()
    {
        std::cout.flush();
    }
};
