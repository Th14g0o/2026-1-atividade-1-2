// servidor_echo.cpp
// Servidor TCP echo — devolve cada mensagem recebida até o cliente desconectar.

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int PORT        = 12345;
const int BUFFER_SIZE = 1024;

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
    if (listen(server_fd, 1) < 0) {
        perror("listen"); close(server_fd); return 1;
    }

    std::cout << "[Echo] Servidor aguardando na porta " << PORT << "...\n";

    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) { perror("accept"); close(server_fd); return 1; }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    std::cout << "[Echo] Cliente conectado: " << client_ip << "\n";

    // Loop echo: recebe e reenvia até o cliente fechar a conexão
    char buffer[BUFFER_SIZE];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            std::cout << "[Echo] Cliente desconectado.\n";
            break;
        }
        std::cout << "[Echo] Recebido: " << buffer << "\n";
        send(client_fd, buffer, bytes, 0);  // devolve a mesma mensagem
    }

    close(client_fd);
    close(server_fd);
    return 0;
}