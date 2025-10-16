#include "server.h"
#include "session.h"
#include <iostream>
#include <memory>
#include <string>

Server::Server(short port)
    : m_io_context(),
    m_acceptor(m_io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
{
    std::cout << "Server object created. Listening on port " << port << ".\n";
}

void Server::Start() {
    DoAccept();
    std::cout << "Server started. Waiting for connections...\n";
    m_io_context.run();
    std::cout << "Server stopped.\n";
}

void Server::Join(std::shared_ptr<Session> session) {
    m_sessions.insert(session);
    std::cout << "Client joined. Total clients: " << m_sessions.size() << std::endl;
}

void Server::Leave(std::shared_ptr<Session> session) {
    m_sessions.erase(session);
    std::cout << "Client left. Total clients: " << m_sessions.size() << std::endl;
}

void Server::Deliver(const std::shared_ptr<const std::string>& msg) {
    // Log the message to the server console
    std::cout << *msg;
    if (!msg->empty() && msg->back() != '\n') {
        std::cout << std::endl;
    }

    // Broadcast the message to all connected clients
    for (auto& session : m_sessions) {
        session->Deliver(msg);
    }
}

void Server::DoAccept() {
    m_acceptor.async_accept(
        [this](const asio::error_code& ec, asio::ip::tcp::socket socket) {
            OnAccept(ec, std::move(socket));
        }
    );
}

void Server::OnAccept(const asio::error_code& ec, asio::ip::tcp::socket socket) {
    if (!ec) {
        std::cout << "Client connected! Remote endpoint: " << socket.remote_endpoint() << std::endl;
        
        // Create a new session and pass a reference to this server
        std::make_shared<Session>(std::move(socket), *this)->Start();
    }
    else {
        std::cerr << "Accept error: " << ec.message() << std::endl;
    }

    DoAccept();
}

