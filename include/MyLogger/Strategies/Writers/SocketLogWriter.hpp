#pragma once

#include "MyLogger/Strategies/LogWriter.hpp"
#include "MyLogger/Strategies/Writers/SocketLogWriterImpl.hpp"

#include <string>
#include <memory>

// *****************************************************************************
//! \brief Thread-safe template-based TCP writer for logging.
//! Uses PIMPL pattern to hide SFML dependency.
//! \tparam LineFormatterType The type of the line formatter.
// *****************************************************************************
template<typename LineFormatterType>
class SocketLogWriter : public LogWriter<SocketLogWriter<LineFormatterType>, LineFormatterType>
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor.
    //-------------------------------------------------------------------------
    SocketLogWriter(const std::string& p_host, unsigned short p_port, LineFormatterType& p_line_formatter)
        : LogWriter<SocketLogWriter<LineFormatterType>, LineFormatterType>(p_line_formatter)
        , m_socket_impl(std::make_unique<SocketLogWriterImpl>(p_host, p_port))
    {
    }

    //-------------------------------------------------------------------------
    //! \brief Write the beginning part of a log message.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void writeBeginImpl(const std::string& p_begin)
    {
        m_socket_impl->ensureConnected();
        if (m_socket_impl->isConnected())
        {
            m_socket_impl->send(p_begin.c_str(), p_begin.size());
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Write the middle part of a log message.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void writeMiddleImpl(const std::string& p_middle)
    {
        if (m_socket_impl->isConnected())
        {
            m_socket_impl->send(p_middle.c_str(), p_middle.size());
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Write the end part of a log message.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void writeEndImpl(const std::string& p_end)
    {
        if (m_socket_impl->isConnected())
        {
            m_socket_impl->send(p_end.c_str(), p_end.size());
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Write a complete formatted message (fallback method).
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void writeImpl(const std::string& p_message)
    {
        m_socket_impl->ensureConnected();
        if (m_socket_impl->isConnected())
        {
            std::string message = p_message + "\n";
            m_socket_impl->send(message.c_str(), message.size());
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Flush the socket output.
    //! Called under lock by base class.
    //-------------------------------------------------------------------------
    void flushImpl()
    {
        // For sockets, no need for an explicit flush
    }

private:
    //! \brief PIMPL wrapper for socket operations
    std::unique_ptr<SocketLogWriterImpl> m_socket_impl;
};