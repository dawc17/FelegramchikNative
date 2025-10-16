#include "session.h"
#include "server.h" // Include Server header
#include <iostream>
#include <memory>

// Modified constructor to accept a reference to the server
Session::Session(asio::ip::tcp::socket socket, Server& server)
    : m_socket(std::move(socket)), m_server(server) {}

void Session::Start() {
    // When a session starts, we join the server's list of sessions
    m_server.Join(shared_from_this());
    DoRead();
}

void Session::Deliver(const std::shared_ptr<const std::string>& msg) {
    bool write_in_progress = !m_write_msgs.empty();
    m_write_msgs.push_back(msg);
    if (!write_in_progress) {
        DoWrite();
    }
}

void Session::DoRead() {
    auto self(shared_from_this());
    m_socket.async_read_some(asio::buffer(m_data, max_length),
        [this, self](const asio::error_code& ec, std::size_t length) {
            OnRead(ec, length);
        });
}

void Session::OnRead(const asio::error_code& ec, std::size_t length) {
    if (!ec) {
        std::string received_msg(m_data, length);
        // Trim newline characters from the received message
        received_msg.erase(received_msg.find_last_not_of("\r\n") + 1);

        if (m_nickname.empty()) {
            // The first message is the nickname
            m_nickname = received_msg;
            auto join_msg = std::make_shared<const std::string>(m_nickname + " has joined the chat!\n");
            m_server.Deliver(join_msg);
        }
        else {
            // Subsequent messages are chat messages
            auto chat_msg = std::make_shared<const std::string>("[" + m_nickname + "]: " + received_msg + "\n");
            m_server.Deliver(chat_msg);
        }

        DoRead();
    }
    else {
        if (ec == asio::error::eof || ec.value() == 10054) {
            // Client disconnected cleanly
            if (!m_nickname.empty()) {
                auto leave_msg = std::make_shared<const std::string>(m_nickname + " has left the chat.\n");
                m_server.Deliver(leave_msg);
            }
        }
        else {
            std::cerr << "Read error: " << ec.message() << std::endl;
        }
        m_server.Leave(shared_from_this());
    }
}

void Session::DoWrite() {
    auto self(shared_from_this());
    asio::async_write(m_socket, asio::buffer(*m_write_msgs.front()),
        [this, self](const asio::error_code& ec, std::size_t length) {
            OnWrite(ec, length);
        });
}

void Session::OnWrite(const asio::error_code& ec, std::size_t /*length*/) {
    if (!ec) {
        m_write_msgs.pop_front();
        if (!m_write_msgs.empty()) {
            DoWrite();
        }
    }
    else {
        std::cerr << "Write error: " << ec.message() << std::endl;
        m_server.Leave(shared_from_this());
    }
}
