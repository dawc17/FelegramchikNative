#include <iostream>
#include <asio.hpp>
#include <functional>
#include "server.h"

// for me: this is a member initializer list. a constructor. essential for oop.
Server::Server(short port)
	: m_io_context(),
	m_acceptor(m_io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
{
	std::cout << "Server object created. Listening on port " << port << ".\n";
}

// implement the start function from server.h
void Server::Start() {
	// create the callback 
	auto callback = std::bind(&Server::OnAccept, this, std::placeholders::_1, std::placeholders::_2);

	m_acceptor.async_accept(callback);

	std::cout << "Server started. Waiting for connections...\n";
	m_io_context.run();
	std::cout << "Server stopped.\n";

}

// define what the callback does
void Server::OnAccept(const asio::error_code& ec, asio::ip::tcp::socket socket) {
	if (ec) {
		std::cerr << "Accept error: " << ec.message() << std::endl;
		return;
	}

	std::cout << "New accept operation in progress...\n";
	auto callback = std::bind(&Server::OnAccept, this, std::placeholders::_1, std::placeholders::_2);
	m_acceptor.async_accept(callback);

	std::cout << "Client connected! Remote endpoint: " << socket.remote_endpoint() << std::endl;
}

