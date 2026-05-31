#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdlib>
#include <netdb.h>
#include <thread>
#include <mutex>
#include <atomic>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;
const int NUM_MENSAGENS = 5;

std::atomic<bool> connected(true);
std::mutex output_mutex;

// Thread para receber mensagens do servidor
void receive_messages(int sock_fd, const std::string& client_name) {
    char buffer[BUFFER_SIZE];

    while (connected) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = recv(sock_fd, buffer, BUFFER_SIZE - 1, 0);

        if (bytes <= 0) {
            {
                std::lock_guard<std::mutex> lock(output_mutex);
                std::cerr << "[" << client_name << "] Conexao encerrada pelo servidor\n";
            }
            connected = false;
            break;
        }

        {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "[" << client_name << "] Resposta: " << buffer;
        }
    }
}

int main(int argc, char* argv[]) {
    // Aceita o nome do cliente como argumento (ex: "cliente_1")
    std::string client_name = (argc > 1) ? argv[1] : "cliente_desconhecido";

    std::cout << std::unitbuf;

    int sock_fd;
    struct sockaddr_in server_addr;

    // Endereço do servidor: usa variável de ambiente ou "server" (hostname Docker)
    const char* server_ip = getenv("SERVER_HOST");
    if (!server_ip) server_ip = "server";

    // Cria socket TCP
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        std::cerr << "[" << client_name << "] Erro ao criar socket\n";
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Resolve o hostname do servidor
    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        // Se não for IP, tenta como hostname via gethostbyname
        struct hostent* he = gethostbyname(server_ip);
        if (!he) {
            std::cerr << "[" << client_name << "] Hostname invalido: " << server_ip << "\n";
            return 1;
        }
        memcpy(&server_addr.sin_addr, he->h_addr_list[0], he->h_length);
    }

    // Conecta ao servidor
    if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "[" << client_name << "] Erro ao conectar em " << server_ip << ":" << PORT << "\n";
        return 1;
    }

    {
        std::lock_guard<std::mutex> lock(output_mutex);
        std::cout << "[" << client_name << "] Conectado ao servidor " << server_ip << ":" << PORT << "\n";
    }

    // Cria thread para receber mensagens
    std::thread receiver_thread(receive_messages, sock_fd, client_name);

    // Thread principal envia mensagens
    for (int i = 1; i <= NUM_MENSAGENS; i++) {
        std::string msg = "Ola do " + client_name + " mensagem #" + std::to_string(i) + "\n";

        send(sock_fd, msg.c_str(), msg.size(), 0);
        {
            std::lock_guard<std::mutex> lock(output_mutex);
            std::cout << "[" << client_name << "] Enviou: " << msg;
        }

        sleep(1); // Pausa de 1s entre mensagens
    }

    // Aguarda a thread de recebimento terminar
    receiver_thread.join();

    {
        std::lock_guard<std::mutex> lock(output_mutex);
        std::cout << "[" << client_name << "] Encerrando conexao\n";
    }
    close(sock_fd);
    return 0;
}
