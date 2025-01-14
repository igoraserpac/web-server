#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/iterativo/iterativo.h"
#include "../include/fork/fork.h"
#include "../include/fila-tarefas/fila-tarefas.h"
#include "../include/select/select.h"

void print_usage(const char *program_name) {
    printf("Uso: %s <modo> <porta>\n", program_name);
    printf("Modos disponíveis:\n");
    printf("  iterativo        - Servidor iterativo\n");
    printf("  fork             - Servidor utilizando fork ou threads\n");
    printf("  fila-tarefas     - Servidor com threads e fila de tarefas\n");
    printf("  select           - Servidor concorrente utilizando select\n");
    printf("Exemplo: %s iterativo 8080\n", program_name);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *mode = argv[1];
    int port = atoi(argv[2]);

    if (port <= 0 || port > 65535) {
        fprintf(stderr, "Erro: A porta deve estar entre 1 e 65535.\n");
        return EXIT_FAILURE;
    }

    if (strcmp(mode, "iterativo") == 0) {
        printf("Iniciando servidor no modo iterativo na porta %d...\n", port);
        start_iterative_server(port);
    } else if (strcmp(mode, "fork") == 0) {
        printf("Iniciando servidor no modo fork/thread na porta %d...\n", port);
        start_fork_server(port);
    } else if (strcmp(mode, "fila-tarefas") == 0) {
        printf("Iniciando servidor no modo fila de tarefas na porta %d...\n", port);
        start_task_queue_server(port, 4);
    } else if (strcmp(mode, "select") == 0) {
        printf("Iniciando servidor no modo select na porta %d...\n", port);
        start_select_server(port);
    } else {
        fprintf(stderr, "Erro: Modo inválido '%s'.\n", mode);
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
