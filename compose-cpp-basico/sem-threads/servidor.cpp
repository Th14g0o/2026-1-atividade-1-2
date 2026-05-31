#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/select.h>

const int PORT = 8080;
const int MAX_CLIENTS = 10;
const int BUFFER_SIZE = 1024;

int main() {
    int server_fd;
    int client_fds[MAX_CLIENTS];
    struct sockaddr_in address;
    fd_set read_fds;
    char buffer[BUFFER_SIZE];

    // Inicializa array de clientes como -1 (vazio)
    for (int i = 0; i < MAX_CLIENTS; i++)
        client_fds[i] = -1;

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
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        std::cerr << "Erro no listen\n";
        return 1;
    }

    std::cout << "Servidor aguardando conexoes na porta "
          << PORT
          << "..."
          << std::endl;

    while (true) {
        // Monta o conjunto de file descriptors para o select()
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        int max_fd = server_fd;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_fds[i] > 0) {
                FD_SET(client_fds[i], &read_fds);
                if (client_fds[i] > max_fd)
                    max_fd = client_fds[i];
            }
        }

        // select() bloqueia até algum fd ficar pronto para leitura
        int activity = select(max_fd + 1, &read_fds, nullptr, nullptr, nullptr);
        if (activity < 0) {
            std::cerr << "Erro no select\n";
            break;
        }

        // Nova conexão entrando no server_fd
        if (FD_ISSET(server_fd, &read_fds)) {
            socklen_t addrlen = sizeof(address);
            int new_fd = accept(server_fd, (struct sockaddr*)&address, &addrlen);
            if (new_fd < 0) {
                std::cerr << "Erro no accept\n";
                continue;
            }

            std::cout << "Nova conexao: fd=" << new_fd
                      << " ip=" << inet_ntoa(address.sin_addr)
                      << " porta=" << ntohs(address.sin_port) << "\n";

            // Adiciona ao array de clientes
            bool added = false;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == -1) {
                    client_fds[i] = new_fd;
                    added = true;
                    break;
                }
            }
            if (!added) {
                std::cout << "Servidor cheio. Conexao recusada.\n";
                close(new_fd);
            }
        }

        // Verifica cada cliente por dados recebidos
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int fd = client_fds[i];
            if (fd < 0) continue;

            if (FD_ISSET(fd, &read_fds)) {
                memset(buffer, 0, BUFFER_SIZE);
                int bytes = read(fd, buffer, BUFFER_SIZE - 1);

                if (bytes <= 0) {
                    // Cliente desconectou
                    std::cout << "Cliente fd=" << fd << " desconectou\n";
                    close(fd);
                    client_fds[i] = -1;
                } else {
                    std::string msg(buffer);
                    std::cout << "fd=" << fd << " recebeu: " << msg;

                    // Eco: devolve a mensagem com prefixo
                    std::string response = "SERVIDOR RECEBEU: " + msg;
                    send(fd, response.c_str(), response.size(), 0);
                }
            }
        }
    }

    close(server_fd);
    return 0;
}