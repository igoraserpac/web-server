# Nome do executável
EXEC = server

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -Iinclude/iterativo -Iinclude/fork -Iinclude/fila-tarefas -Iinclude/select -Iinclude/game_logic

# Diretórios
SRC_DIR = src
INCLUDE_DIR = include
OBJ_DIR = obj

# Fontes e objetos
SRCS = $(SRC_DIR)/main.c \
       $(INCLUDE_DIR)/iterativo/iterativo.c \
       $(INCLUDE_DIR)/fork/fork.c \
       $(INCLUDE_DIR)/fila-tarefas/fila-tarefas.c \
       $(INCLUDE_DIR)/select/select.c \
       $(INCLUDE_DIR)/game_logic/game_logic.c
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o)

# Regras principais
.PHONY: all clean

all: $(EXEC)

# Regra para criar o executável
$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Regra para compilar os arquivos .c em .o
$(OBJ_DIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza dos arquivos gerados
clean:
	rm -rf $(OBJ_DIR) $(EXEC)
