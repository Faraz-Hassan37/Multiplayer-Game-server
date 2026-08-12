#include <SFML/Network.hpp>
#include <iostream>

int main()
{
    sf::TcpListener listener;
    sf::TcpSocket client;
    bool clientConnected = false;

    const unsigned short PORT = 53000;

    // Listen on port
    if (listener.listen(PORT) != sf::Socket::Done)
    {
        std::cout << "Failed to start server\n";
        return 1;
    }

    std::cout << "Server started. Waiting for client...\n";

    // Accept ONLY ONE client
    if (listener.accept(client) == sf::Socket::Done)
    {
        clientConnected = true;
        std::cout << "Client connected: "
            << client.getRemoteAddress() << std::endl;
    }

    // Stop listening after one client
    listener.close();

    sf::Packet packet;

    while (clientConnected)
    {
        packet.clear();

        // Receive data
        if (client.receive(packet) == sf::Socket::Done)
        {
            std::string message;
            packet >> message;

            std::cout << "Client says: " << message << std::endl;

            // Send response
            sf::Packet reply;
            reply << "Server received: " + message;
            client.send(reply);
        }
        else
        {
            std::cout << "Client disconnected\n";
            clientConnected = false;
        }
    }

    return 0;
}
