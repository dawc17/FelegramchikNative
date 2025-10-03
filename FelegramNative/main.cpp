#include "server.h" 
#include <iostream>

int main() {
    try {
        Server my_server(1337);

        my_server.Start();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}