// servidor_chat.cpp
// Servidor de chat TCP multi-cliente usando std::thread.
// Cada cliente que conecta ganha uma thread; mensagens são broadcast para todos.

#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <algorithm>
#include <unistd.h>
#include <arpa/inet.h>

const int PORT        = 12345;
const int BUFFER_SIZE = 1024;
const int MAX_CLIENTS = 10;

// Lista de sockets ativos e mutex para proteção
std::vector<int> clients;
std::mutex       clients_mutex;

// Envia uma mensagem para todos os clientes, exceto o remetente
void broadcast(const std::string& msg, int sender_fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    for (int fd : clients) {
        if (fd != sender_fd) {
            send(fd, msg.c_str(), msg.size(), 0);
        }
    }
}

// Remove um cliente da lista
void remove_client(int fd) {
    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(std::remove(clients.begin(), clients.end(), fd), clients.end());
}

// Função executada em thread para cada cliente conectado
void handle_client(int client_fd, std::string client_ip) {
    std::cout << "[Chat] " << client_ip << " entrou no chat.\n";

    std::string entrada_msg = client_ip + " entrou no chat.\n";
    broadcast(entrada_msg, client_fd);

    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes <= 0) {
            // Cliente desconectou
            std::cout << "[Chat] " << client_ip << " saiu do chat.\n";
            std::string saida_msg = client_ip + " saiu do chat.\n";
            remove_client(client_fd);
            broadcast(saida_msg, -1);
            break;
        }

        std::string msg = "[" + client_ip + "]: " + std::string(buffer, bytes);
        std::cout << msg;
        broadcast(msg, client_fd);
    }

    close(client_fd);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in address{};
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port        = htons(PORT);

    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind"); close(server_fd); return 1;
    }
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("listen"); close(server_fd); return 1;
    }

    std::cout << "[Chat] Servidor de chat iniciado na porta " << PORT << "\n";
    std::cout << "[Chat] Aguardando clientes...\n";

    while (true) {
        sockaddr_in client_addr{};
        socklen_t   client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) { perror("accept"); continue; }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));

        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            clients.push_back(client_fd);
        }

        // Criar thread para o cliente (detach: sem necessidade de join)
        std::thread t(handle_client, client_fd, std::string(client_ip));
        t.detach();
    }

    close(server_fd);
    return 0;
}