#include "MyLogger/Strategies/Writers/SocketLogWriterImpl.hpp"

#include <SFML/Network.hpp>

// *****************************************************************************
//! \brief Actual implementation that holds SFML objects.
// *****************************************************************************
class SocketLogWriterImpl::Impl
{
public:

    Impl(const std::string& p_host, unsigned short p_port)
        : m_host(p_host), m_port(p_port), m_connected(false)
    {
        connect();
    }

    void connect()
    {
        m_connected = m_socket.connect(m_host, m_port) == sf::Socket::Status::Done;
    }

    sf::TcpSocket m_socket;
    std::string m_host;
    unsigned short m_port;
    bool m_connected;
};

// *****************************************************************************
// SocketLogWriterImpl implementation
// *****************************************************************************

SocketLogWriterImpl::SocketLogWriterImpl(const std::string& p_host, unsigned short p_port)
    : m_pImpl(std::make_unique<Impl>(p_host, p_port))
{
}

SocketLogWriterImpl::~SocketLogWriterImpl() = default;

bool SocketLogWriterImpl::send(const char* p_data, size_t p_size)
{
    if (!m_pImpl->m_connected)
    {
        return false;
    }

    return m_pImpl->m_socket.send(p_data, p_size) == sf::Socket::Status::Done;
}

bool SocketLogWriterImpl::isConnected() const
{
    return m_pImpl->m_connected;
}

bool SocketLogWriterImpl::ensureConnected()
{
    if (!m_pImpl->m_connected)
    {
        m_pImpl->connect();
    }
    return m_pImpl->m_connected;
}