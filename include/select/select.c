#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>
#include "select.h"
#include "../game_logic/game_logic.h"

#define BACKLOG 5       // Número máximo de conexões na fila de espera
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 100 // Número máximo de clientes que podem se conectar simultaneamente

void start_select_server(int port) {
    int server_fd, client_fd, max_fd, activity;
    int client_sockets[MAX_CLIENTS];
    fd_set readfds; // Conjunto de descritores para monitorar
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    
    // Inicializa os sockets dos clientes como vazios
    for (int i = 0; i < MAX_CLIENTS; i++) {
        client_sockets[i] = 0;
    }

    // Cria o socket do servidor
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }

    // Configura o endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Faz o bind do socket à porta especificada
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erro no bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Coloca o servidor em modo de escuta
    if (listen(server_fd, BACKLOG) == -1) {
        perror("Erro no listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Servidor concorrente com select rodando na porta %d...\n", port);

    // Loop principal para lidar com conexões
    while (1) {
        // Limpa e configura o conjunto de descritores
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds); // Adiciona o descritor do servidor
        max_fd = server_fd;

        // Adiciona os sockets dos clientes ao conjunto de descritores
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sock = client_sockets[i];
            if (sock > 0) {
                FD_SET(sock, &readfds);
            }
            if (sock > max_fd) {
                max_fd = sock;
            }
        }

        // Aguarda atividade nos descritores
        activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("Erro no select");
            continue;
        }

        // Verifica se há uma nova conexão no socket do servidor
        if (FD_ISSET(server_fd, &readfds)) {
            client_len = sizeof(client_addr);
            client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
            if (client_fd == -1) {
                perror("Erro ao aceitar conexão");
                continue;
            }

            printf("Nova conexão aceita de %s:%d\n",
                   inet_ntoa(client_addr.sin_addr),
                   ntohs(client_addr.sin_port));

            // Adiciona o novo socket à lista de clientes
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (client_sockets[i] == 0) {
                    client_sockets[i] = client_fd;
                    printf("Adicionado ao array de clientes na posição %d\n", i);
                    break;
                }
            }
        }

        // Lida com I/O de clientes existentes
        for (int i = 0; i < MAX_CLIENTS; i++) {
            int sock = client_sockets[i];
            if (FD_ISSET(sock, &readfds)) {
                // Processa o jogo da velha com o cliente
                process_game(sock);

                // Conexão encerrada após o término do jogo
                printf("Conexão encerrada pelo cliente no socket %d\n", sock);
                close(sock);
                client_sockets[i] = 0;
            }
        }
    }

    // Fecha o socket do servidor
    close(server_fd);
}
