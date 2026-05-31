# Servidor e Cliente sem Threads

## Visão Geral

Implementação de um **servidor Echo** que atende múltiplos clientes simultaneamente **sem usar threads**, utilizando **multiplexing com `select()`**.

## Abordagem: Select Multiplexing

### Como Funciona?

Em vez de criar uma thread para cada cliente, o servidor usa `select()` para monitorar **múltiplos file descriptors** (sockets) em um único loop:

1. **select()** aguarda até que algum socket tenha dados para ler
2. Verifica qual socket ficou "pronto"
3. Processa apenas o socket que tem dados
4. Volta a monitorar todos os sockets

## Bibliotecas Principais

| Biblioteca | Função | Por quê? |
|-----------|--------|---------|
| `<sys/socket.h>` | Criar sockets TCP | Comunicação de rede |
| `<netinet/in.h>` | Estruturas de endereço (sockaddr_in) | Configurar porta e IP |
| `<arpa/inet.h>` | Conversão de endereços (inet_pton, inet_ntoa) | Converter IP string em binário e o contrario |
| `<sys/select.h>` | Função `select()` | Monitorar múltiplos sockets |
| `<unistd.h>` | close(), read(), write() | I/O e controle de arquivo |
| `<netdb.h>` | gethostbyname() | Resolver hostnames (ex: "server") |

## Fluxo Principal

### Servidor (`servidor.cpp`)

1. Criar socket TCP
2. Bind + Listen na porta 8080
3. LOOP INFINITO:
  - Adiciona server_fd e todos os client_fds ao select()
  - select() bloqueia até algo ficar "pronto"
  - Se server_fd está pronto -> Nova conexão (accept)
  - Se client_fd está pronto -> Ler dados + Enviar eco
  - Se bytes <= 0 -> Cliente desconectou (close)

### Cliente (`cliente.cpp`)

1. Resolver hostname "server" → IP
2. Conectar ao servidor
3. FOR 5 vezes:
  - Enviar mensagem
  - Aguardar resposta (bloqueante)
  - Esperar 1 segundo
4. Fechar conexão

## Compilação com Docker

```bash
# Build das imagens
docker-compose build

# Executar
docker-compose up

# Parar
docker-compose down
```

O `Dockerfile` compila com:
```dockerfile
g++ -std=c++17 -Wall -o program arquivo.cpp
```

- `-std=c++17` → C++ 17 (permite std::string, etc)
- `-Wall` → Mostrar todos os warnings

## Comandos de Socket Importantes

| Comando | Uso |
|---------|-----|
| `socket()` | Criar novo socket |
| `bind()` | Associar socket a porta |
| `listen()` | Marcar como servidor, aceitar conexões |
| `accept()` | Aceitar nova conexão (retorna novo fd) |
| `connect()` | Cliente conecta ao servidor |
| `select()` | **Aguardar múltiplos sockets ficar prontos** |
| `read()` | Ler dados do socket |
| `send()` / `write()` | Enviar dados |
| `close()` | Fechar socket |

---

## Glossário

### **fd (File Descriptor)**
Número inteiro que identifica um arquivo ou socket aberto.

### **Socket**
Ponto de comunicação entre dois programas em rede. Como um "ponto de entrada" para enviar/receber dados.
Exemplo: Um socket TCP conecta cliente a servidor.

### **TCP (Transmission Control Protocol)**
Protocolo de comunicação confiável que garante entrega de dados na ordem correta.
Diferente de UDP (mais rápido mas sem garantias).

### **bind()**
Associa um socket a um endereço e porta específicos.
Exemplo: "Este socket escuta na porta 8080".

### **listen()**
Marca um socket como servidor e prepara para aceitar conexões de clientes.

### **accept()**
Aceita uma nova conexão de um cliente.
Retorna um novo fd para comunicação com aquele cliente específico.

### **connect()**
Cliente se conecta ao servidor usando IP e porta.

### **Bloqueante (Blocking)**
Operação que pausa a execução até terminar.

### **select()**
Função que monitora múltiplos sockets simultaneamente em um único loop.
Aguarda até algum ficar "pronto" (tem dados para ler ou aceita conexão).

### **Multiplexing**
Técnica de monitorar múltiplos recursos (sockets) com uma única execução.
Vantagem: economiza memória (sem muitas threads).

