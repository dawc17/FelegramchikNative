#include <iostream>
#include <asio.hpp>

int main()
{
    try {
        // this has to be done every time i create an asio project, simply initializes
        asio::io_context io_context;

        // this line creates an endpoint that listens for ipv4, tcp connections on port 1337
        asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), 1337);

        // an acceptor is created here to actually accept these connections
        asio::ip::tcp::acceptor acceptor(io_context, endpoint);

        asio::ip::tcp::socket socket(io_context);

        std::cout << "Listening on port 1337..." << std::endl;
        // this is a blocking call. nothing in the program will proceed until a client connects
        acceptor.accept(socket);
        std::cout << "Client connected." << std::endl;

        // create a buffer using an array of a maximum of 1024 characters (fixed, cannot be changed)
        std::array<char, 1024> myBuffer;
        // this function stores everything it read into a size_t variable
        size_t bytesRead = socket.read_some(asio::buffer(myBuffer));

        std::cout << "Successfully read" << bytesRead << " bytes." << std::endl;

        // a high level function that doesnt stop writing to the socket until all the data is sent.
        // it takes in the target as the first argument, and the size of the data as the second.
        asio::write(socket, asio::buffer(myBuffer, bytesRead));
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return 0;
}


