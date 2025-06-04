#pragma once

#include <string>
#include <memory>

// *****************************************************************************
//! \brief Implementation class for socket operations (PIMPL pattern).
//! Hides SFML dependency from the header.
// *****************************************************************************
class SocketLogWriterImpl
{
public:
    //-------------------------------------------------------------------------
    //! \brief Constructor.
    //! \param p_host The host to connect to.
    //! \param p_port The port to connect to.
    //-------------------------------------------------------------------------
    SocketLogWriterImpl(const std::string& p_host, unsigned short p_port);

    //-------------------------------------------------------------------------
    //! \brief Destructor.
    //-------------------------------------------------------------------------
    ~SocketLogWriterImpl();

    //-------------------------------------------------------------------------
    //! \brief Copy constructor (deleted).
    //-------------------------------------------------------------------------
    SocketLogWriterImpl(const SocketLogWriterImpl&) = delete;

    //-------------------------------------------------------------------------
    //! \brief Assignment operator (deleted).
    //-------------------------------------------------------------------------
    SocketLogWriterImpl& operator=(const SocketLogWriterImpl&) = delete;

    //-------------------------------------------------------------------------
    //! \brief Send data through the socket.
    //! \param p_data The data to send.
    //! \param p_size The size of the data.
    //! \return True if successful, false otherwise.
    //-------------------------------------------------------------------------
    bool send(const char* p_data, size_t p_size);

    //-------------------------------------------------------------------------
    //! \brief Check if the socket is connected.
    //! \return True if connected, false otherwise.
    //-------------------------------------------------------------------------
    bool isConnected() const;

    //-------------------------------------------------------------------------
    //! \brief Ensure the socket connection is established.
    //! \return True if connected, false otherwise.
    //-------------------------------------------------------------------------
    bool ensureConnected();

private:
    class Impl;
    std::unique_ptr<Impl> m_pImpl;
};