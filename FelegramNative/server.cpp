#include <iostream>
#include <asio.hpp>
#include <memory>
#include "server.h"
#include "session.h" // Include the Session header file

// for me: this is a member initializer list. a constructor. essential for oop.
Server::Server(short port)
	: m_io_context(),
	m_acceptor(m_io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
{
	std::cout << "Server object created. Listening on port " << port << ".\n";
}

// implement the start function from server.h
void Server::Start() {
	DoAccept();

	std::cout << "Server started. Waiting for connections...\n";
	m_io_context.run();
	std::cout << "Server stopped.\n";
}

void Server::DoAccept() {
	m_acceptor.async_accept(
		[this](const asio::error_code& ec, asio::ip::tcp::socket socket) {
			OnAccept(ec, std::move(socket));
		}
	);
}

// define what the callback does
void Server::OnAccept(const asio::error_code& ec, asio::ip::tcp::socket socket) {
	if (!ec) {
		std::cout << "Client connected! Remote endpoint: " << socket.remote_endpoint() << std::endl;
		
		// create a new session for this client and start it
		std::make_shared<Session>(std::move(socket))->Start();
	}
	else {
		std::cerr << "Accept error: " << ec.message() << std::endl;
	}

	DoAccept();
}

