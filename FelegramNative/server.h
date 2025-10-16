#pragma once
#include <iostream>
#include <asio.hpp>
#include "session.h"

class Server {
public:
	Server(short port);

	void Start();

private:
	// define all members so they can be implemented in cpp
	void DoAccept();
	void OnAccept(const asio::error_code& ec, asio::ip::tcp::socket socket);

	asio::io_context m_io_context;
	asio::ip::tcp::acceptor m_acceptor;
};