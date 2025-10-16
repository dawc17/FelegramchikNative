#pragma once
#include <memory>
#include <asio.hpp>
#include <iostream>

class Session : public std::enable_shared_from_this<Session> {
public:
	Session(asio::ip::tcp::socket socket);

	void Start();

private:
	void DoRead();
	void DoWrite(std::size_t length);

	asio::ip::tcp::socket m_socket;
	enum { max_length = 1024 };
	char m_data[max_length];
};
