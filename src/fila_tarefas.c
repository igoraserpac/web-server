#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <signal.h>
#include "../include/fila_tarefas.h"
#include "../include/request_handler.h"

#define BACKLOG 5       // Número máximo de conexões na fila de espera
#define BUFFER_SIZE 1024
#define QUEUE_SIZE 50   // Tamanho máximo da fila de tarefas

typedef struct {
    int sockets[QUEUE_SIZE];
    int front;
    int rear;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} task_queue_t;

static task_queue_t queue;

int server_fd_fila = -1;

void handle_sigint_fila() {
    if (server_fd_fila != -1) {
        close(server_fd_fila);
        printf("\nServidor encerrado e porta liberada.\n");
    }
    exit(0);
}

void enqueue(int socket) {
    pthread_mutex_lock(&queue.lock);

    while (queue.count == QUEUE_SIZE) {
        pthread_cond_wait(&queue.not_full, &queue.lock);
    }

    queue.sockets[queue.rear] = socket;
    queue.rear = (queue.rear + 1) % QUEUE_SIZE;
    queue.count++;

    pthread_cond_signal(&queue.not_empty);
    pthread_mutex_unlock(&queue.lock);
}

int dequeue() {
    pthread_mutex_lock(&queue.lock);

    while (queue.count == 0) {
        pthread_cond_wait(&queue.not_empty, &queue.lock);
    }

    int socket = queue.sockets[queue.front];
    queue.front = (queue.front + 1) % QUEUE_SIZE;
    queue.count--;

    pthread_cond_signal(&queue.not_full);
    pthread_mutex_unlock(&queue.lock);

    return socket;
}

void *worker_thread() {
    while (1) {
        // Dequeue: obtém o descritor de socket do próximo cliente na fila
        int client_fd = dequeue();

        printf("Thread iniciando o processamento do jogo para o cliente.\n");

        // Processamento de requisição
        process_request(client_fd);

        // Fecha a conexão com o cliente após o término do envio
        close(client_fd);
        printf("Conexão encerrada pela thread.\n");
    }
    return NULL;
}

void start_task_queue_server(int port, int thread_count) {
    int client_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;
    pthread_t threads[thread_count];

    // Registrar tratador de sinal
    signal(SIGINT, handle_sigint_fila);

    // Inicializa a fila de tarefas
    queue.front = queue.rear = queue.count = 0;
    pthread_mutex_init(&queue.lock, NULL);
    pthread_cond_init(&queue.not_empty, NULL);
    pthread_cond_init(&queue.not_full, NULL);

    // Cria as threads trabalhadoras
    for (int i = 0; i < thread_count; i++) {
        if (pthread_create(&threads[i], NULL, worker_thread, NULL) != 0) {
            perror("Erro ao criar thread");
            exit(EXIT_FAILURE);
        }
    }

    // Criação do socket
    server_fd_fila = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_fila == -1) {
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }

    // Configuração do endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind do socket
    if (bind(server_fd_fila, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erro no bind");
        close(server_fd_fila);
        exit(EXIT_FAILURE);
    }

    // Escutando conexões
    if (listen(server_fd_fila, BACKLOG) == -1) {
        perror("Erro no listen");
        close(server_fd_fila);
        exit(EXIT_FAILURE);
    }

    printf("Servidor com fila de tarefas rodando na porta %d...\n", port);

    // Loop principal para aceitar conexões e colocá-las na fila
    while (1) {
        client_len = sizeof(client_addr);
        client_fd = accept(server_fd_fila, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd == -1) {
            perror("Erro ao aceitar conexão");
            continue;
        }

        printf("Conexão aceita de %s:%d\n",
               inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port));

        enqueue(client_fd);
    }

    // Liberação de recursos
    close(server_fd_fila);
    pthread_mutex_destroy(&queue.lock);
    pthread_cond_destroy(&queue.not_empty);
    pthread_cond_destroy(&queue.not_full);
}