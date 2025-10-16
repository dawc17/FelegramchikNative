#pragma once

#include "server.h"
#include <asio.hpp>
#include <memory>
#include <string> // Include for std::string
#include <deque>

class Session : public std::enable_shared_from_this<Session> {
public:
    // Modified constructor to accept a reference to the server
    Session(asio::ip::tcp::socket socket, Server& server);

    void Start();

    // This method will be called by the Server to deliver a message to this client
    void Deliver(const std::shared_ptr<const std::string>& msg);

private:
    void DoRead();
    void OnRead(const asio::error_code& ec, std::size_t length);
    void DoWrite();
    void OnWrite(const asio::error_code& ec, std::size_t /*length*/);

    static const int max_length = 1024;
    asio::ip::tcp::socket m_socket;
    char m_data[max_length];
    Server& m_server; // Reference to the server
    std::deque<std::shared_ptr<const std::string>> m_write_msgs;
    std::string m_nickname;
};
