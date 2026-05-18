FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    # Toolchain C++
    build-essential \
    g++ \
    gdb \
    make \
    cmake \
    # Utilitários de rede (netcat, ss, telnet para testar sockets)
    netcat-openbsd \
    iproute2 \
    iputils-ping \
    # Qualidade de vida
    git \
    curl \
    vim \
    bash-completion \
    && rm -rf /var/lib/apt/lists/*

# Usuário não-root (padrão do VS Code Dev Containers)
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=${USER_UID}

RUN groupadd --gid ${USER_GID} ${USERNAME} \
    && useradd --uid ${USER_UID} --gid ${USER_GID} -m ${USERNAME} \
    && apt-get update && apt-get install -y sudo \
    && echo "${USERNAME} ALL=(ALL) NOPASSWD:ALL" > /etc/sudoers.d/${USERNAME} \
    && chmod 0440 /etc/sudoers.d/${USERNAME} \
    && rm -rf /var/lib/apt/lists/*

USER ${USERNAME}
WORKDIR /workspace