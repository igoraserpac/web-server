#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include "../include/iterativo.h"
#include "../include/request_handler.h"

#define BACKLOG 5 // Número máximo de conexões na fila de espera
#define BUFFER_SIZE 1024

int server_fd_iterative = -1;

void handle_sigint_iterative() {
    if (server_fd_iterative != -1) {
        close(server_fd_iterative);
        printf("\nServidor encerrado e porta liberada.\n");
    }
    exit(0);
}

void start_iterative_server(int port) {
    int client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    // Registrar tratador de sinal
    signal(SIGINT, handle_sigint_iterative);

    // Criação do socket
    server_fd_iterative = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_iterative == -1) {
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }

    // Configuração do endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind do socket
    if (bind(server_fd_iterative, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erro no bind");
        close(server_fd_iterative);
        exit(EXIT_FAILURE);
    }

    // Escutando conexões
    if (listen(server_fd_iterative, BACKLOG) == -1) {
        perror("Erro no listen");
        close(server_fd_iterative);
        exit(EXIT_FAILURE);
    }

    printf("Servidor iterativo rodando na porta %d...\n", port);

    // Loop principal para aceitar e processar conexões
    while (1) {
        client_len = sizeof(client_addr);
        client_fd = accept(server_fd_iterative, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("Erro ao aceitar conexão");
            continue;
        }

        printf("Conexão aceita de %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        // Processamento de requisição
        process_request(client_fd);

        // Fecha a conexão com o cliente após o término do envio
        close(client_fd);
        printf("Conexão encerrada.\n");
    }

    // Fecha o socket do servidor
    close(server_fd_iterative);
}

