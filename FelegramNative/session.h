#pragma once

#include <asio.hpp>
#include <memory>
#include <string> // Include for std::string

// Forward declaration to avoid circular dependency
class Server;

class Session : public std::enable_shared_from_this<Session>
{
public:
    // Modified constructor to accept a reference to the server
    Session(asio::ip::tcp::socket socket, Server& server);
    void Start();
    // This method will be called by the Server to deliver a message to this client
    void Deliver(const std::shared_ptr<const std::string>& msg);

private:
    void DoRead();

    asio::ip::tcp::socket m_socket;
    Server& m_server; // Reference to the server
    enum { max_length = 1024 };
    char m_data[max_length];
};
