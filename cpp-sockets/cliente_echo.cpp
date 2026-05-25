// cliente_echo.cpp
// Cliente TCP echo — envia mensagens digitadas e exibe o eco do servidor.

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const char* HOST        = "127.0.0.1";
const int   PORT        = 12345;
const int   BUFFER_SIZE = 1024;

int main() {
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) { perror("socket"); return 1; }

    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(PORT);
    inet_pton(AF_INET, HOST, &server_addr.sin_addr);

    if (connect(sock_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect"); close(sock_fd); return 1;
    }

    std::cout << "[Echo] Conectado. Digite mensagens (Ctrl+C ou 'sair' para encerrar):\n";

    char buffer[BUFFER_SIZE];
    std::string linha;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, linha)) break;   // EOF
        if (linha == "sair") break;
        if (linha.empty()) continue;

        // Enviar
        send(sock_fd, linha.c_str(), linha.size(), 0);

        // Receber eco
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) { std::cout << "Servidor desconectou.\n"; break; }
        std::cout << "[Eco] " << buffer << "\n";
    }

    close(sock_fd);
    return 0;
}