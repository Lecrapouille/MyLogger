#pragma once

#include "MyLogger/Strategies/LogWriter.hpp"
#include <SFML/Network.hpp>
#include <string>

// *****************************************************************************
//! \brief Thread-safe template-based TCP writer for logging.
//! Direct SFML implementation without PIMPL pattern.
//! \tparam LineFormatterType The type of the line formatter.
// *****************************************************************************
template <typename LineFormatterType>
class SocketLogWriter
    : public LogWriter<SocketLogWriter<LineFormatterType>, LineFormatterType>
{
public:

    //-------------------------------------------------------------------------
    //! \brief Constructor.
    //! \param p_host The host to connect to.
    //! \param p_port The port to connect to.
    //! \param p_line_formatter Reference to the line formatter.
    //-------------------------------------------------------------------------
    SocketLogWriter(const std::string& p_host,
                    unsigned short p_port,
                    LineFormatterType& p_line_formatter)
        : LogWriter<SocketLogWriter<LineFormatterType>, LineFormatterType>(
              p_line_formatter),
          m_host(p_host),
          m_port(p_port)
    {
        connect();
    }

    //-------------------------------------------------------------------------
    //! \brief Destructor.
    //-------------------------------------------------------------------------
    ~SocketLogWriter() = default;

    //-------------------------------------------------------------------------
    //! \brief Write a complete formatted message.
    //! Called under lock by base class.
    //! \param p_message The message to write.
    //-------------------------------------------------------------------------
    void writeImpl(const std::string& p_message)
    {
        ensureConnected();
        if (m_connected)
        {
            std::string message = p_message + "\n";
            send(message.c_str(), message.size());
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

    //-------------------------------------------------------------------------
    //! \brief Establish socket connection.
    //-------------------------------------------------------------------------
    void connect()
    {
        m_connected =
            m_socket.connect(m_host, m_port) == sf::Socket::Status::Done;
    }

    //-------------------------------------------------------------------------
    //! \brief Ensure the socket connection is established.
    //-------------------------------------------------------------------------
    void ensureConnected()
    {
        if (!m_connected)
        {
            connect();
        }
    }

    //-------------------------------------------------------------------------
    //! \brief Send data through the socket.
    //! \param p_data The data to send.
    //! \param p_size The size of the data.
    //-------------------------------------------------------------------------
    void send(const char* p_data, size_t p_size)
    {
        if (m_connected)
        {
            m_socket.send(p_data, p_size);
        }
    }

    //! \brief SFML TCP socket
    sf::TcpSocket m_socket;
    //! \brief Host to connect to
    std::string m_host;
    //! \brief Port to connect to
    unsigned short m_port;
    //! \brief Connection status
    bool m_connected = false;
};