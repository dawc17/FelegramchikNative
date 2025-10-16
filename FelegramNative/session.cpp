#include "session.h"
#include "server.h" // Include server header
#include <string>
#include <iostream>
#include <memory> // Include for std::make_shared

Session::Session(asio::ip::tcp::socket socket, Server& server)
    : m_socket(std::move(socket)), m_server(server)
{
}

void Session::Start() {
    m_server.Join(shared_from_this()); // Add session to the server's list
    DoRead();
}

void Session::DoRead() {
    auto self(shared_from_this());
    m_socket.async_read_some(
        asio::buffer(m_data, max_length),
        [this, self](const asio::error_code& ec, std::size_t length) {
            if (!ec) {
                std::string client_ip = m_socket.remote_endpoint().address().to_string();
                int client_port = m_socket.remote_endpoint().port();
                
                std::string message(m_data, length);
                
                // Create the formatted message and deliver it via the server
                auto formatted_msg = std::make_shared<const std::string>(
                    "[" + client_ip + ":" + std::to_string(client_port) + "] " + message
                );
                m_server.Deliver(formatted_msg);
                
                DoRead(); // Continue reading the next message
            }
            else {
                m_server.Leave(shared_from_this()); // Remove session on disconnect/error
            }
        }
    );
}

void Session::Deliver(const std::shared_ptr<const std::string>& msg) {
    auto self(shared_from_this());
    asio::async_write(
        m_socket,
        asio::buffer(*msg),
        [this, self, msg](const asio::error_code& /*ec*/, std::size_t /*length*/) {
            // The shared_ptr 'msg' is captured by value, keeping the string alive.
        }
    );
}
