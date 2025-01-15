# Nome do compilador
CC = gcc

# Diretórios
INCLUDE_DIR = include
SRC_DIR = src
OBJ_DIR = obj
FILES_DIR = files

# Flags de compilação
CFLAGS = -Wall -Wextra -Werror -I$(INCLUDE_DIR)

# Arquivos fonte
SOURCES = $(SRC_DIR)/main.c \
          $(SRC_DIR)/iterativo.c \
          $(SRC_DIR)/fork.c \
          $(SRC_DIR)/fila-tarefas.c \
          $(SRC_DIR)/select.c \
          $(SRC_DIR)/request_handler.c

# Arquivos objeto na pasta obj
OBJECTS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES))

# Nome do executável
TARGET = server

# Regras
all: $(TARGET)

# Criação do executável
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Compilação dos arquivos .c para .o na pasta obj
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Criação da pasta obj, se não existir
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Limpeza dos arquivos objeto e do executável
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: all
	./$(TARGET) iterativo 8080

.PHONY: all clean run
