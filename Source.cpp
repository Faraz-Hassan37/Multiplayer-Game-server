#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

///////////////////////////////////////////////////////////////////////////////
// NETWORK STRUCTS
///////////////////////////////////////////////////////////////////////////////
struct NetPlayerState {
    int id;
    float x;
    float y;
    int state;        // 0=Idle,1=Move,2=Jump,3=Attack
    bool facingRight;
};

///////////////////////////////////////////////////////////////////////////////
// COMMON COMMUNICATION PROTOCOL
///////////////////////////////////////////////////////////////////////////////
enum PacketType {
    PKT_PLAYER_STATE = 0,
    PKT_PLAYER_HIT = 1,
    PKT_PLAYER_DEAD = 2
};

///////////////////////////////////////////////////////////////////////////////
// QUEUE IMPLEMENTATION
///////////////////////////////////////////////////////////////////////////////
template<typename T, int SIZE>
class Queue {
public:
    Queue() : front(0), rear(0), count(0) {}

    bool push(const T& v) {
        if (count == SIZE) return false;
        data[rear] = v;
        rear = (rear + 1) % SIZE;
        count++;
        return true;
    }

    bool pop(T& v) {
        if (count == 0) return false;
        v = data[front];
        front = (front + 1) % SIZE;
        count--;
        return true;
    }

private:
    T data[SIZE];
    int front, rear, count;
};

///////////////////////////////////////////////////////////////////////////////
// QUEUED PACKET WRAPPER
///////////////////////////////////////////////////////////////////////////////
struct QueuedPacket {
    sf::Packet packet;
    sf::TcpSocket* sender;
};

///////////////////////////////////////////////////////////////////////////////
// GAME SERVER
///////////////////////////////////////////////////////////////////////////////
class GameServer {
public:
    void run() {
        listener.listen(54000);
        std::cout << "[SERVER] Running on port 54000\n";

        while (true) {
            sf::TcpSocket* client = new sf::TcpSocket;

            if (listener.accept(*client) == sf::Socket::Done) {
                client->setBlocking(false);

                int id = nextClientID++;
                sockets.push_back(client);

                sf::Packet idPacket;
                idPacket << id;
                client->send(idPacket);

                {
                    std::lock_guard<std::mutex> lock(logMutex);
                    std::cout << "[CONNECT] Player " << id << " connected\n";
                }

                threads.emplace_back(&GameServer::clientThread, this, client, id);
            }
        }
    }

private:
    sf::TcpListener listener;
    std::vector<sf::TcpSocket*> sockets;
    std::vector<std::thread> threads;

    Queue<QueuedPacket, 256> packetQueue;

    std::mutex queueMutex;
    std::mutex logMutex;

    int nextClientID = 0;

    ///////////////////////////////////////////////////////////////////////////
    // CLIENT THREAD
    ///////////////////////////////////////////////////////////////////////////
    void clientThread(sf::TcpSocket* socket, int id) {
        while (true) {
            sf::Packet packet;
            sf::Socket::Status status = socket->receive(packet);

            if (status == sf::Socket::Disconnected) {
                std::lock_guard<std::mutex> lock(logMutex);
                std::cout << "[DISCONNECT] Player " << id << " disconnected\n";
                return;
            }

            if (status == sf::Socket::Done) {
                QueuedPacket qp;
                qp.packet = packet;
                qp.sender = socket;

                {
                    std::lock_guard<std::mutex> lock(queueMutex);
                    packetQueue.push(qp);
                }
            }

            processQueuedPackets();
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // PROCESS QUEUED PACKETS
    ///////////////////////////////////////////////////////////////////////////
    void processQueuedPackets() {
        QueuedPacket qp;

        while (true) {
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (!packetQueue.pop(qp))
                    break;
            }

            sf::Packet& packet = qp.packet;
            sf::TcpSocket* sender = qp.sender;

            int type;
            packet >> type;

            // --- PLAYER STATE ---
            if (type == PKT_PLAYER_STATE) {
                NetPlayerState s;
                packet >> s.id >> s.x >> s.y >> s.state >> s.facingRight;

                {
                    std::lock_guard<std::mutex> lock(logMutex);
                    std::cout << "[STATE] Player " << s.id
                        << " (" << s.x << "," << s.y << ") "
                        << "State=" << s.state << "\n";
                }

                sf::Packet out;
                out << PKT_PLAYER_STATE
                    << s.id << s.x << s.y << s.state << s.facingRight;

                broadcastToOthers(out, sender);
            }

            // --- PLAYER HIT ---
            else if (type == PKT_PLAYER_HIT) {
                int attackerID, targetID;
                packet >> attackerID >> targetID;

                {
                    std::lock_guard<std::mutex> lock(logMutex);
                    std::cout << "[HIT] Player " << attackerID
                        << " hit Player " << targetID << "\n";
                }

                sf::Packet out;
                out << PKT_PLAYER_HIT << attackerID << targetID;

                broadcastToOthers(out, sender);
            }

            // --- PLAYER DEAD ---
            else if (type == PKT_PLAYER_DEAD) {
                int deadID;
                packet >> deadID;

                sf::Packet out;
                out << PKT_PLAYER_DEAD << deadID;
                broadcastToOthers(out, sender);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // BROADCAST TO ALL EXCEPT SENDER
    ///////////////////////////////////////////////////////////////////////////
    void broadcastToOthers(sf::Packet& packet, sf::TcpSocket* sender) {
        for (auto& sock : sockets) {
            if (sock != sender) {
                sock->send(packet);
            }
        }
    }
};

///////////////////////////////////////////////////////////////////////////////
// MAIN
///////////////////////////////////////////////////////////////////////////////
int main() {
    GameServer server;
    server.run();
    return 0;
}