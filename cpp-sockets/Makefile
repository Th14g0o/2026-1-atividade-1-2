# Makefile — cpp-sockets
# Compila todos os exemplos de uma vez ou individualmente.

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -g

TARGETS = servidor_basico cliente_basico \
          servidor_echo   cliente_echo   \
          servidor_chat   cliente_chat

all: $(TARGETS)

servidor_basico: servidor_basico.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

cliente_basico: cliente_basico.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

servidor_echo: servidor_echo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

cliente_echo: cliente_echo.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

servidor_chat: servidor_chat.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

cliente_chat: cliente_chat.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)

.PHONY: all clean