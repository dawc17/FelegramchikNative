#include "session.h"
#include <string>

Session::Session(asio::ip::tcp::socket socket)
	: m_socket(std::move(socket))
{
}

void Session::Start() {
	std::cout << "Session started for client: " << m_socket.remote_endpoint() << std::endl;
	DoRead();
}

void Session::DoRead() {
	auto self(shared_from_this());
	m_socket.async_read_some(
		asio::buffer(m_data, max_length),
		[this, self](const asio::error_code& ec, std::size_t length) {
			if (!ec) {
				// display the message with client IP
				std::string client_ip = m_socket.remote_endpoint().address().to_string();
				int client_port = m_socket.remote_endpoint().port();
				
				// convert received data to string
				std::string message(m_data, length);
				
				std::cout << "[" << client_ip << ":" << client_port << "] " << message;
				
				// check if message doesn't end with newline, add one for better formatting
				if (length > 0 && m_data[length - 1] != '\n') {
					std::cout << std::endl;
				}
				
				DoWrite(length);
			}
			else if (ec != asio::error::eof) {
				std::cerr << "Read error: " << ec.message() << std::endl;
			}
			else {
				std::cout << "Client disconnected: " << m_socket.remote_endpoint() << std::endl;
			}
		}
	);
}

void Session::DoWrite(std::size_t length) {
	auto self(shared_from_this());
	asio::async_write(
		m_socket,
		asio::buffer(m_data, length),
		[this, self](const asio::error_code& ec, std::size_t /*bytes_written*/) {
			if (!ec) {
				DoRead(); // continue reading after writing
			}
			else {
				std::cerr << "Write error: " << ec.message() << std::endl;
			}
		}
	);
}
