# Servidor e Cliente com Threads

## Visão Geral

Implementação de um **servidor Echo** que atende múltiplos clientes simultaneamente **usando threads**. Cada cliente conectado é processado em sua própria thread.

## Abordagem: uma Thread por Cliente

### Como Funciona?

Quando um cliente se conecta:

1. **accept()** retorna um novo socket para aquele cliente
2. **Cria uma thread** dedicada para lidar com este cliente
3. Thread trata **enviar/receber** daquele cliente específico
4. Quando cliente desconecta, thread encerra

## Bibliotecas Principais

| Biblioteca | Função | Por quê? |
|-----------|--------|---------|
| `<sys/socket.h>` | Criar sockets TCP | Comunicação de rede |
| `<netinet/in.h>` | Estruturas de endereço (sockaddr_in) | Configurar porta e IP |
| `<arpa/inet.h>` | Conversão de endereços | Converter IP string em binário e o contrario |
| `<unistd.h>` | close(), read(), write() | I/O básico |
| `<netdb.h>` | gethostbyname() | Resolver hostnames (ex: "server_threads") |
| **`<thread>`** | **std::thread** | **Criar threads** |
| **`<mutex>`** | **std::mutex, std::lock_guard** | **Sincronizar acesso a stdout** |
| **`<atomic>`** | **std::atomic<bool>** | **Flag thread-safe entre threads** |

## Fluxo Principal

### Servidor (`servidor.cpp`)

1. Criar socket TCP
2. Bind + Listen na porta 8080
3. LOOP INFINITO:
  - accept() aguarda nova conexão (bloqueante)
  - Cria std::thread executando handle_client()
  - thread.detach() → deixa rodar independente
   
handle_client() em cada THREAD:
- LOOP enquanto cliente conectado:
  - Ler dados do cliente
  - Enviar eco com "SERVIDOR RECEBEU: "
  - Se bytes <= 0 → Desconectar
- close() socket e encerra thread

### Cliente (`cliente.cpp`)

1. Conectar ao servidor
2. Cria thread de RECEBIMENTO (receive_messages)
  - Aguarda respostas do servidor (bloqueante)
3. Thread PRINCIPAL envia mensagens:
  - FOR 5 vezes:
    - Enviar mensagem
    - Esperar 1 segundo
4. join() aguarda thread de recebimento terminar
5. Fechar conexão

## Sincronização Entre Threads

### Mutex (`std::mutex`)

Protege acesso ao stdout para evitar mensagens entrelaçadas:

```cpp
std::mutex output_mutex;

// Uso:
{
    std::lock_guard<std::mutex> lock(output_mutex);
    std::cout << "Mensagem segura\n";  // Só uma thread por vez
}
// lock_guard destrói e libera mutex automaticamente
```

### Atomic (`std::atomic<bool>`)

Flag thread-safe para sinalizar desconexão:

```cpp
std::atomic<bool> connected(true);

// Thread 1:
connected = false;  // Seguro, sem mutex

// Thread 2:
while (connected) { ... }  // Lê valor atualizado
```

## Compilação com Docker

```bash
# Build das imagens
docker-compose build

# Executar
docker-compose up

# Parar
docker-compose down
```

O `Dockerfile` compila com **-pthread**:
```dockerfile
g++ -std=c++17 -Wall -pthread -o program arquivo.cpp
```

- `-std=c++17` → C++ 17 (permite `<thread>`, `<mutex>`, etc)
- `-pthread` → **Linkar com biblioteca de threads (POSIX threads)**
- `-Wall` → Mostrar todos os warnings

## Comandos de Thread Importantes

| Comando | Uso |
|---------|-----|
| `std::thread t(func, args...)` | Criar thread executando `func` |
| `t.join()` | Aguardar thread `t` terminar |
| `t.detach()` | Deixar thread rodar independentemente |
| `std::mutex m` | Mutex para sincronização |
| `std::lock_guard<std::mutex>` | Auto-lock/unlock (RAII) |
| `std::atomic<T>` | Tipo thread-safe sem mutex |

---

## Glossário 

### **fd (File Descriptor)**
Número inteiro que identifica um arquivo ou socket aberto. Exemplo: `server_fd = 3`, `client_fd = 4`.
Por padrão: 0=stdin, 1=stdout, 2=stderr. Arquivos/sockets começam em 3.

### **Socket**
Ponto de comunicação entre dois programas em rede. Como um "ponto de entrada" para enviar/receber dados.

### **TCP (Transmission Control Protocol)**
Protocolo de comunicação confiável que garante entrega de dados na ordem correta.
Diferente de UDP (mais rápido mas sem garantias).

### **accept()**
Aceita uma nova conexão de um cliente.
Retorna um novo fd para comunicação com aquele cliente específico.

### **connect()**
Cliente se conecta ao servidor usando IP e porta.

### **Bloqueante (Blocking)**
Operação que pausa a execução até terminar..

### **Thread**
Fluxo de execução independente dentro de um programa.
Cada cliente tem sua própria thread → executa simultaneamente.
Vantagem: Código simples. Desvantagem: Usa mais memória.

### **Mutex (Mutual Exclusion)**
Mecanismo que garante que apenas uma thread acesse um recurso por vez.
Protege variáveis/stdout de acesso simultâneo (evita "race conditions").
Exemplo: Duas threads imprimindo simultaneamente → mensagens entrelaçadas → mutex previne isso.

### **Lock/Unlock**
**Lock**: Trava exclusiva (thread A entra, outras esperam).
**Unlock**: Libera a trava (próxima thread pode entrar).
`std::lock_guard` faz isso automaticamente (RAII).

### **Atomic**
Tipo de dado que é "thread-safe" sem precisar de mutex.
Operações simples (leitura/escrita) são seguras automaticamente.
Exemplo: `std::atomic<bool> connected` pode ser lida/modificada por múltiplas threads sem problemas.

### **join()**
Aguarda uma thread terminar antes de continuar.
Exemplo: `receiver_thread.join()` → programa pausa até thread de recebimento encerrar.

### **detach()**
Deixa uma thread rodar independentemente sem esperar terminar.
Programa continua enquanto a thread roda em background.

### **POSIX Threads**
Padrão UNIX de threads. A flag `-pthread` no compilador ativa suporte.
Implementado em Linux, macOS, etc.
