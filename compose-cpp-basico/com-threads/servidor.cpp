#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <vector>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

std::mutex clients_mutex;
int client_count = 0;

// Função para lidar com cada cliente em sua própria thread
void handle_client(int client_fd, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    
    {
        std::lock_guard<std::mutex> lock(clients_mutex);
        client_count++;
        std::cout << "Cliente conectado: fd=" << client_fd
                  << " ip=" << inet_ntoa(client_addr.sin_addr)
                  << " porta=" << ntohs(client_addr.sin_port)
                  << " (Total de clientes: " << client_count << ")\n";
    }

    while (true) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes = read(client_fd, buffer, BUFFER_SIZE - 1);

        if (bytes <= 0) {
            // Cliente desconectou
            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                std::cout << "Cliente fd=" << client_fd << " desconectou\n";
                client_count--;
            }
            break;
        }

        std::string msg(buffer);
        {
            std::lock_guard<std::mutex> lock(clients_mutex);
            std::cout << "fd=" << client_fd << " recebeu: " << msg;
        }

        // Eco: devolve a mensagem com prefixo
        std::string response = "SERVIDOR RECEBEU: " + msg;
        send(client_fd, response.c_str(), response.size(), 0);
    }

    close(client_fd);
}

int main() {
    std::cout << std::unitbuf;
    
    int server_fd;
    struct sockaddr_in address;
    std::vector<std::thread> threads;

    // Cria socket TCP
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        std::cerr << "Erro ao criar socket\n";
        return 1;
    }

    // Permite reusar a porta imediatamente após restart
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Configura endereço
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind e listen
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        std::cerr << "Erro no bind\n";
        return 1;
    }
    if (listen(server_fd, 10) < 0) {
        std::cerr << "Erro no listen\n";
        return 1;
    }

    std::cout << "Servidor aguardando conexoes na porta " << PORT << " (com threads)...\n";

    while (true) {
        socklen_t addrlen = sizeof(address);
        int new_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (new_fd < 0) {
            std::cerr << "Erro no accept\n";
            continue;
        }

        // Cria uma thread para lidar com este cliente
        threads.emplace_back(handle_client, new_fd, address);
        threads.back().detach(); // Deixa a thread rodar independentemente
    }

    close(server_fd);
    return 0;
}
