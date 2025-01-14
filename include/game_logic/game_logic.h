#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#define BOARD_SIZE 3

static char board[BOARD_SIZE][BOARD_SIZE];
static int current_player = 1;

// Inicializa o tabuleiro
void initialize_board();

// Exibe o tabuleiro no formato de string
void render_board(char *buffer);

// Verifica se uma jogada é válida
int is_valid_move(int row, int col);

// Processa a jogada de um jogador
int make_move(int player, int row, int col);

// Verifica se há um vencedor
int check_winner();

// Processa a lógica do jogo para um cliente
void process_game(int client_fd);

#endif