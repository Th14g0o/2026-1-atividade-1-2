// servidor_basico.cpp
// Servidor TCP básico — aceita uma conexão, envia uma mensagem e encerra.

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

const int PORT = 12345;
const int BUFFER_SIZE = 1024;

int main() {
    // Criar socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Erro ao criar socket");
        return 1;
    }

    // Permitir reusar a porta após restart
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configurar endereço
    sockaddr_in address{};
    address.sin_family      = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;   // aceita qualquer interface
    address.sin_port        = htons(PORT);

    // Bind — associar socket à porta
    if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
        perror("Erro no bind");
        close(server_fd);
        return 1;
    }

    // Listen — colocar em modo de escuta
    if (listen(server_fd, 1) < 0) {
        perror("Erro no listen");
        close(server_fd);
        return 1;
    }

    std::cout << "[Servidor] Aguardando conexão na porta " << PORT << "...\n";

    // Accept — aceitar conexão do cliente
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int client_fd = accept(server_fd, (sockaddr*)&client_addr, &client_len);
    if (client_fd < 0) {
        perror("Erro no accept");
        close(server_fd);
        return 1;
    }

    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
    std::cout << "[Servidor] Cliente conectado: " << client_ip << "\n";

    // Receber mensagem do cliente
    char buffer[BUFFER_SIZE] = {};
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes > 0) {
        std::cout << "[Servidor] Mensagem recebida: " << buffer << "\n";
    }

    // Enviar resposta
    const char* resposta = "Olá! Mensagem recebida com sucesso.";
    send(client_fd, resposta, strlen(resposta), 0);
    std::cout << "[Servidor] Resposta enviada. Encerrando.\n";

    close(client_fd);
    close(server_fd);
    return 0;
}