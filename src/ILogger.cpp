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

#include "MyLogger/ILogger.hpp"
#include <cstdarg>

namespace mylogger {

//------------------------------------------------------------------------------
const char *ILogger::strtime()
{
    currentTime();
    return m_buffer_time;
}

//------------------------------------------------------------------------------
void ILogger::currentDate()
{
    time_t current_time = time(nullptr);

    strftime(m_buffer_time, sizeof (m_buffer_time), "[%Y/%m/%d]", localtime(&current_time));
}

//------------------------------------------------------------------------------
void ILogger::currentTime()
{
    time_t current_time = time(nullptr);

    strftime(m_buffer_time, sizeof (m_buffer_time), "[%H:%M:%S]", localtime(&current_time));
}

//------------------------------------------------------------------------------
void ILogger::log(std::ostream *stream, enum Severity severity, const char* format, ...)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    int n;
    va_list params;

    m_severity = severity;
    m_stream = stream;
    beginOfLine();
    va_start(params, format);
    n = vsnprintf(m_buffer, c_buffer_size - 2u, format, params);
    va_end(params);

    // Add a '\n' if missing
    if ('\n' != m_buffer[n - 1])
    {
        m_buffer[n] = '\n';
        m_buffer[n + 1] = '\0';
    }

    write(m_buffer);

    m_stream = nullptr;
}

//------------------------------------------------------------------------------
void ILogger::log(const char* format, ...)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    va_list params;

    va_start(params, format);
    vsnprintf(m_buffer, c_buffer_size, format, params);
    va_end(params);
    write(m_buffer);
}

} // namespace mylogger
