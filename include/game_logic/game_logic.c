#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "game_logic.h"

// Inicializa o tabuleiro
void initialize_board() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board[i][j] = ' ';
        }
    }
}

// Exibe o tabuleiro no formato de string
void render_board(char *buffer) {
    sprintf(buffer, 
        " %c | %c | %c \n"
        "---+---+---\n"
        " %c | %c | %c \n"
        "---+---+---\n"
        " %c | %c | %c \n",
        board[0][0], board[0][1], board[0][2],
        board[1][0], board[1][1], board[1][2],
        board[2][0], board[2][1], board[2][2]);
}

// Verifica se uma jogada é válida
int is_valid_move(int row, int col) {
    return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE && board[row][col] == ' ';
}

// Processa a jogada de um jogador
int make_move(int player, int row, int col) {
    if (!is_valid_move(row, col)) {
        return 0;
    }
    board[row][col] = (player == 1) ? 'X' : 'O';
    return 1;
}

// Verifica se há um vencedor
int check_winner() {
    // Verifica linhas e colunas
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            return (board[i][0] == 'X') ? 1 : 2;
        }
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
            return (board[0][i] == 'X') ? 1 : 2;
        }
    }

    // Verifica diagonais
    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return (board[0][0] == 'X') ? 1 : 2;
    }
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return (board[0][2] == 'X') ? 1 : 2;
    }

    // Verifica se há empate
    int draw = 1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == ' ') {
                draw = 0;
            }
        }
    }
    return draw ? 0 : -1;
}

// Processa a lógica do jogo para um cliente
void process_game(int client_fd) {
    char buffer[1024];
    int player = current_player;
    current_player = (current_player % 2) + 1;

    initialize_board();

    while (1) {
        render_board(buffer);
        send(client_fd, buffer, strlen(buffer), 0);

        // Envia a mensagem de turno
        sprintf(buffer, "Jogador %d, sua vez (formato: linha coluna):\n", player);
        send(client_fd, buffer, strlen(buffer), 0);

        // Recebe a jogada
        int row, col;
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            close(client_fd);
            return;
        }
        buffer[bytes_received] = '\0';
        sscanf(buffer, "%d %d", &row, &col);

        // Processa a jogada
        if (!make_move(player, row, col)) {
            sprintf(buffer, "Jogada inválida! Tente novamente.\n");
            send(client_fd, buffer, strlen(buffer), 0);
            continue;
        }

        // Verifica o estado do jogo
        int winner = check_winner();
        if (winner != -1) {
            render_board(buffer);
            send(client_fd, buffer, strlen(buffer), 0);

            if (winner == 0) {
                sprintf(buffer, "Empate!\n");
            } else {
                sprintf(buffer, "Jogador %d venceu!\n", winner);
            }
            send(client_fd, buffer, strlen(buffer), 0);
            close(client_fd);
            return;
        }
    }
}
