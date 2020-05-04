//=====================================================================
// MyLogger: A basic logger.
// Copyright 2018 Quentin Quadrat <lecrapouille@gmail.com>
//
// This file is part of MyLogger.
//
// MyLogger is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with MyLogger.  If not, see <http://www.gnu.org/licenses/>.
//=====================================================================

#ifndef MYLOGGER_ILOGGER_HPP
#  define MYLOGGER_ILOGGER_HPP

#  include <mutex>
#  include <stdarg.h>
#  include <fstream>
#  include <cassert>
#  include <sstream>
#  include <string>
#  include <iostream>

namespace tool { namespace log {

// *****************************************************************************
//! \brief Different severity enumerate
// *****************************************************************************
enum Severity
{
    None, Info, Debug, Warning, Failed, Error, Signal, Exception,
    Catch, Fatal, MaxLoggerSeverity = Fatal
};

// *****************************************************************************
//! \brief Interface class for loggers.
// *****************************************************************************
class ILogger
{
public:

    //! \brief Virtual destructor because of virtual methods.
    virtual ~ILogger() = default;

    //! \brief entry point for logging data. This method formats data into
    //! m_buffer up to 1024 chars.
    void log(const char* format, ...);

    //! \brief entry point for logging data. This method formats data into
    //! m_buffer.
    void log(std::ostream *stream, enum Severity severity, const char* format, ...);

    //! \brief entry point for logging data. This method formats data into
    //! m_buffer.
    template <class T> ILogger& operator<<(const T& tolog);

protected:

    //! \brief Return the date or time get by methods currentDate() or
    //! currentTime()
    const char *strtime();

    //! \brief Get the current date (year, month, day). Store the date as string
    //! inside m_buffer_time.
    void currentDate();

    //! \brief Get the current time (hour, minute, second). Store the date as
    //! string inside m_buffer_time.
    void currentTime();

private:

    //! \brief Virtual method used for storing m_buffer in the media you wish.
    virtual void write(std::string const& message) = 0;

    //! \brief Virtual method used for storing m_buffer in the media you wish.
    virtual void write(const char *message, const int length = -1) = 0;

    //! \brief Virtual method for formating the begining of the line log (ie.
    //! severity, date, filename ...).
    virtual void beginOfLine() = 0;

protected:

    //! \brief Max char for formating a line of logs.
    constexpr static const uint32_t c_buffer_size = 1024u;

    //! \brief Buffer used for storing a line of logs.
    char m_buffer[c_buffer_size];

    //! \brief Store date information
    char m_buffer_time[32];

    //! \brief Protect write against concurrency.
    std::mutex m_mutex;

    //! \brief Current log severity.
    enum Severity m_severity = None;

    //! \brief Memorize the stream for the method write() when log(std::ostream*).
    std::ostream *m_stream = nullptr;
};

template <class T> ILogger& ILogger::operator<<(const T& to_log)
{
    std::ostringstream stream;
    stream << to_log;
    write(stream.str());

    return *this;
}

} } // namespace tool::log

#endif /* MYLOGGER_ILOGGER_HPP */
