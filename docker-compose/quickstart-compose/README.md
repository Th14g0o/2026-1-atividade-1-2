# Quickstart

Seguindo o [Docker Docs - QuickStart](https://docs.docker.com/compose/gettingstarted).

## Arquivos
- `cliente-compose/`
  - `Dockerfile`: Configurações do ambiente do container
  - `.dockerignore`: arquivos ignorados para o buid do container
  - `requirements.txt`: dependencias pro docker baixar(no nosso caso tambem pro C++)
  - `.env`: Senhas para usar no acesso ao Redis(Especie de memoria para armazenamento rapido, tipo chache).
  - `app.cpp`: Conexão com Redis e reposta http.

## Adaptações em relação ao quickstart

O quickstart da documentação do Docker usa python. Como nosso projeto deve ser C++, pedi pro GPT agerar proporcionais e fui adaptando conforme erros. Por isso pode ter coisas desnecessarias no estado atual. 

- `requirements.txt`: Ajustado para ter bibliotecas proporcionais ao Flask e Redis do Python no C++
- `app.cpp`: Para usar comandos C++ e biblotecas proporcionais ao do quickstart.
- `.dockerignore`: Para ignorar os resultados de compilação do C++.
- `Dockerfile`: Instalar C++ e suas dependencias.

## Como iniciar

> Seguindo o "quickstart" da documentação

1. **Ir para diretorio**
```bash
$ cd docker-compose\quickstart-compose\
``` 

2. **Inciar compose**
```bash
$ docker compose up
```
3. **Verificar se funcionou**
- Vá para o navegador e coleque nele: http://localhost:8000
- Deve aparecer: `Hello from Docker! I have been seen 1 time(s).`, definido no app.cpp.
  - Recarregar a pagina significa somar + 1.

4. **Parar compose**:

```bash
$ d
$ docker compose down
```

> Reinicia a contagem. Pois ainda não tem nenhum tipo de persistencia.
