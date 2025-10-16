#pragma once

#include <asio.hpp>
#include <memory>
#include <set>
#include <string> // Include for std::string

// Forward declaration
class Session;

class Server
{
public:
    Server(short port);
    void Start();

    void Join(std::shared_ptr<Session> session);
    void Leave(std::shared_ptr<Session> session);
    // Use a shared_ptr to manage the lifetime of the message string
    void Deliver(const std::shared_ptr<const std::string>& msg);

private:
    void DoAccept();
    void OnAccept(const asio::error_code& ec, asio::ip::tcp::socket socket);

    asio::io_context m_io_context;
    asio::ip::tcp::acceptor m_acceptor;
    
    std::set<std::shared_ptr<Session>> m_sessions;
};