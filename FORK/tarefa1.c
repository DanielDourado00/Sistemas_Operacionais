#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include "checkPassword.h"

#define PASSWORD_LENGTH 12

void bruteForce(int start) {
    char attempt[4] = {0}; // String nula terminada
    for (char c1 = 33; c1 <= 126; c1++) {
        for (char c2 = 33; c2 <= 126; c2++) {
            for (char c3 = 33; c3 <= 126; c3++) {
                attempt[0] = c1;
                attempt[1] = c2;
                attempt[2] = c3;

                if (checkPassword(attempt, start) == 0) {
                    printf("Processo %d encontrou posição %d-%d: %s\n", getpid(), start, start + 2, attempt);
                    return; // Encontrou os caracteres; sair da função
                }
            }
        }
    }
}

int main(int argc, char *argv[]) {
    int useFork = 0;

    // Verifica se o argumento -f foi passado
    if (argc > 1 && strcmp(argv[1], "-f") == 0) {
        useFork = 1;
    }

    if (useFork) {
        // Usar fork para dividir tarefas entre processos
        for (int i = 0; i < 4; i++) {
            pid_t pid = fork();
            if (pid == 0) {
                // Processo filho
                bruteForce(i * 3); // Cada filho trabalha em um bloco de 3 posições
                exit(0); // Sai após completar sua parte
            } else if (pid < 0) {
                perror("Erro ao criar processo");
                exit(1);
            }
        }

        // Processo pai espera todos os filhos
        while (wait(NULL) > 0);
    } else {
        // Sem fork: realiza as tarefas sequencialmente
        for (int i = 0; i < 4; i++) {
            bruteForce(i * 3);
        }
    }

    return 0;
}
