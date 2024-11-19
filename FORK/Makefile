# Nome do programa
TARGET = tarefa1

# Arquivos do programa
OBJECTS = tarefa1.c checkPassword.o

# Comando de compilação
CC = gcc

# Flags de compilação
CFLAGS = -Wall

# Regra padrão
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Limpeza
clean:
	rm -f $(TARGET)
