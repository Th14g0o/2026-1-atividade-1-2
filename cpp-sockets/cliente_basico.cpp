// cliente_basico.cpp
// Cliente TCP básico — conecta ao servidor, envia uma mensagem e lê a resposta.

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const char* HOST = "127.0.0.1";
const int   PORT = 12345;
const int   BUFFER_SIZE = 1024;

int main() {
    // Criar socket
    int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("Erro ao criar socket");
        return 1;
    }

    // Configurar endereço do servidor
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(PORT);
    if (inet_pton(AF_INET, HOST, &server_addr.sin_addr) <= 0) {
        std::cerr << "Endereço inválido: " << HOST << "\n";
        close(sock_fd);
        return 1;
    }

    // Conectar ao servidor
    if (connect(sock_fd, (sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erro ao conectar");
        close(sock_fd);
        return 1;
    }

    std::cout << "[Cliente] Conectado ao servidor " << HOST << ":" << PORT << "\n";

    // Enviar mensagem
    const char* mensagem = "Olá, servidor!";
    send(sock_fd, mensagem, strlen(mensagem), 0);
    std::cout << "[Cliente] Mensagem enviada: " << mensagem << "\n";

    // Receber resposta
    char buffer[BUFFER_SIZE] = {};
    ssize_t bytes = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        std::cout << "[Cliente] Resposta do servidor: " << buffer << "\n";
    }

    close(sock_fd);
    return 0;
}