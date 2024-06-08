#include <crow.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_set>

void broadcastMessages(std::unordered_set<crow::websocket::connection*>& clients) {
    while (true) {
        // Broadcast "Hello, World!" to all connected clients every 5 seconds
        for (auto client : clients) {
            client->send_text("Hello, World!");
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main() {
    crow::SimpleApp app;

    // Define a set to keep track of connected clients
    std::unordered_set<crow::websocket::connection*> clients;

    std::thread broadcastThread(broadcastMessages, std::ref(clients));

    CROW_ROUTE(app, "/ws")
        .websocket()
        .onopen([&clients](crow::websocket::connection& conn) {
        clients.insert(&conn);
        std::cout << "New connection established" << std::endl;
            })
        .onclose([&clients](crow::websocket::connection& conn, const std::string& /*reason*/) {
                clients.erase(&conn);
                std::cout << "Connection closed" << std::endl;
            });

            // Start the server on port 8080
            app.port(8080).multithreaded().run();

            // Wait for the broadcast thread to finish (which will never happen)
            broadcastThread.join();

            return 0;
}
