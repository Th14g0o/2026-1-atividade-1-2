// cliente_chat.cpp
// Cliente de chat TCP — usa duas threads: uma para ler do servidor,
// outra para ler a entrada do usuário e enviar.

#include <iostream>
#include <cstring>
#include <string>
#include <thread>
#include <atomic>
#include <unistd.h>
#include <arpa/inet.h>

const char* HOST        = "127.0.0.1";
const int   PORT        = 12345;
const int   BUFFER_SIZE = 1024;

std::atomic<bool> rodando{true};

// Thread que fica recebendo mensagens do servidor e imprimindo
void receive_loop(int sock_fd) {
    char buffer[BUFFER_SIZE];
    while (rodando) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytes = recv(sock_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) {
            if (rodando) std::cout << "\n[Chat] Desconectado do servidor.\n";
            rodando = false;
            break;
        }
        // Limpa a linha atual e imprime a mensagem recebida
        std::cout << "\r" << buffer << "> " << std::flush;
    }
}

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

    std::cout << "[Chat] Conectado ao servidor. Digite 'sair' para encerrar.\n> ";
    std::cout << std::flush;

    // Thread de recebimento (background)
    std::thread recv_thread(receive_loop, sock_fd);
    recv_thread.detach();

    // Loop principal: lê input do usuário e envia
    std::string linha;
    while (rodando && std::getline(std::cin, linha)) {
        if (linha == "sair") break;
        if (linha.empty()) { std::cout << "> " << std::flush; continue; }

        send(sock_fd, linha.c_str(), linha.size(), 0);
        std::cout << "> " << std::flush;
    }

    rodando = false;
    close(sock_fd);
    return 0;
}