#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../include/request_handler.h"

#define BUFFER_SIZE 1024


void handle_sigint() {
    if (server_fd != -1) {
        close(server_fd);
        printf("\nServidor encerrado e porta liberada.\n");
    }
    exit(0);
}

void process_request(int client_fd) {
    char buffer[BUFFER_SIZE];
    FILE *file;
    size_t bytes_read;

    // Recebe o nome do arquivo do cliente
    int received = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
    if (received <= 0) {
        perror("Erro ao receber o nome do arquivo");
        return;
    }
    buffer[received] = '\0';
    buffer[strcspn(buffer, "\r\n")] = 0; // Remove quebra de linha ou retorno de cursor
    printf("Nome do arquivo solicitado: %s\n", buffer);

    // Abre o arquivo solicitado
    char file_path[BUFFER_SIZE];
    snprintf(file_path, BUFFER_SIZE + 6, "files/%s", buffer);

    file = fopen(file_path, "rb");

    if (!file) {
        perror("Erro ao abrir o arquivo");
        const char *error_msg = "Erro: arquivo não encontrado\n";
        send(client_fd, error_msg, strlen(error_msg), 0);
        return;
    }

    // Envia o conteúdo do arquivo para o cliente
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send(client_fd, buffer, bytes_read, 0) == -1) {
            perror("Erro ao enviar o arquivo");
            break;
        }
    }
    fclose(file);
    printf("Arquivo enviado com sucesso.\n");
}
