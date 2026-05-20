# syntax=docker/dockerfile:1
FROM ubuntu:24.04
# Define diretório de trabalho
WORKDIR /code
# Instala compilador e dependências
RUN apt-get update && apt-get install -y \
    g++ \
    cmake \
    make \
    git \
    libasio-dev \
    libssl-dev \
    wget \
    unzip \
    libhiredis-dev
# Crow
RUN git clone https://github.com/CrowCpp/Crow.git
# redis-plus-plus 
RUN git clone https://github.com/sewenew/redis-plus-plus.git && \
    cd redis-plus-plus && \
    mkdir build && cd build && \
    cmake .. && \
    make -j$(nproc) && \
    make install && \
    ldconfig
# Copia código-fonte
COPY . .
# Compila aplicação
RUN g++ app.cpp -o app \
    -I./Crow/include \
    -std=c++17 \
    -pthread \
    -lredis++ \
    -lhiredis
# Expõe porta HTTP
EXPOSE 5000
# Comando inicial do container
CMD ["./app"]