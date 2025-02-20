/* simulador.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>     // Necessário para strcmp e memcpy
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define TLB_SIZE 16          // Tamanho da TLB
#define NUM_PAGES 256        // Número total de páginas lógicas
#define PAGE_SIZE 256        // Tamanho da página em bytes
#define OFFSET_MASK 0xFF     // Máscara para extrair o offset (8 bits)
#define OFFSET_BITS 8        // Número de bits do offset

// Estrutura para entrada da TLB com flag de validade
typedef struct {
    int logical;
    int physical;
    int valid;
} tlb_entry;

// Estrutura para armazenar informações de cada quadro físico (frame)
// Usada para implementar os algoritmos FIFO e LRU.
typedef struct {
    int logical_page;       // Página lógica atualmente carregada (-1 se vazio)
    int load_time;          // Tempo de carga (para FIFO)
    int last_access_time;   // Último acesso (para LRU)
} frame_info;

int main(int argc, char* argv[]) {
    // Verifica se os parâmetros foram passados corretamente.
    // Uso: ./simulador addresses.txt <Quadros> <Alg Subst Página>
    if(argc != 4) {
       fprintf(stderr, "Uso: %s addresses.txt <Quadros> <Alg Subst Página>\n", argv[0]);
       exit(1);
    }
    char *addresses_file = argv[1];

    // Lê o número de quadros (deve ser 128 ou 256)
    int num_frames = atoi(argv[2]);
    if(num_frames != 128 && num_frames != 256) {
        fprintf(stderr, "Erro: O parâmetro 'Quadros' deve ser 128 ou 256.\n");
        exit(1);
    }

    // Lê o algoritmo de substituição (deve ser FIFO ou LRU em letras maiúsculas)
    char *alg = argv[3];
    if(strcmp(alg, "FIFO") != 0 && strcmp(alg, "LRU") != 0) {
        fprintf(stderr, "Erro: O parâmetro 'Alg Subst Página' deve ser FIFO ou LRU (em letras maiúsculas).\n");
        exit(1);
    }

    // Abre o arquivo BACKING_STORE.bin (presume-se que esteja na mesma pasta)
    int backing_fd = open("BACKING_STORE.bin", O_RDONLY);
    if(backing_fd < 0) {
        perror("Erro ao abrir BACKING_STORE.bin");
        exit(1);
    }
    int backing_size = NUM_PAGES * PAGE_SIZE;
    char *backing = mmap(0, backing_size, PROT_READ, MAP_PRIVATE, backing_fd, 0);
    if(backing == MAP_FAILED) {
        perror("Erro ao mapear BACKING_STORE.bin");
        exit(1);
    }

    // Abre o arquivo de endereços (por exemplo, "addresses.txt")
    FILE *addr_fp = fopen(addresses_file, "r");
    if(addr_fp == NULL) {
        perror("Erro ao abrir arquivo de endereços");
        exit(1);
    }

    // Aloca a memória física
    char *physical_memory = malloc(num_frames * PAGE_SIZE);
    if(physical_memory == NULL) {
        perror("Erro ao alocar memória física");
        exit(1);
    }

    // Inicializa a TLB (todas as entradas inválidas)
    tlb_entry tlb[TLB_SIZE];
    for(int i = 0; i < TLB_SIZE; i++) {
        tlb[i].valid = 0;
    }

    // Inicializa a tabela de páginas (-1 indica que a página não está em memória)
    int pagetable[NUM_PAGES];
    for(int i = 0; i < NUM_PAGES; i++) {
        pagetable[i] = -1;
    }

    // Cria e inicializa a tabela de quadros físicos
    frame_info *frames = malloc(num_frames * sizeof(frame_info));
    if(frames == NULL) {
        perror("Erro ao alocar tabela de quadros");
        exit(1);
    }
    for(int i = 0; i < num_frames; i++) {
        frames[i].logical_page = -1;
        frames[i].load_time = -1;
        frames[i].last_access_time = -1;
    }

    int tlb_next = 0;         // Próxima posição para inserir na TLB (FIFO circular)
    int next_free_frame = 0;  // Próximo quadro livre
    int time_counter = 0;     // Contador global para tempo (para FIFO e LRU)
    int total_addresses = 0;
    int tlb_hits = 0;
    int page_faults = 0;

    // Abre o arquivo de saída "correct.txt"
    FILE *out_fp = fopen("correct.txt", "w");
    if(out_fp == NULL) {
        perror("Erro ao abrir arquivo de saída correct.txt");
        exit(1);
    }

    // Processa cada linha do arquivo de endereços
    char line[256];
    while(fgets(line, sizeof(line), addr_fp) != NULL) {
        // Remove o caractere de nova linha, se existir
        line[strcspn(line, "\n")] = '\0';

        // Verifica se a linha é um comando especial
        if(strcmp(line, "PageTable") == 0) {
            fprintf(out_fp, "###########\nPágina - Quadro - Bit Validade\n###########\n");
            for (int i = 0; i < NUM_PAGES; i++) {
                // Se a página está carregada, o bit de validade é 1; caso contrário, 0.
                if(pagetable[i] != -1)
                    fprintf(out_fp, "%d - %d - 1\n", i, pagetable[i]);
                else
                    fprintf(out_fp, "%d - %d - 0\n", i, pagetable[i]);
            }
            continue;  // Pula para a próxima linha
        } else if(strcmp(line, "TLB") == 0) {
            fprintf(out_fp, "************\nPágina - Quadro\n************\n");
            for (int i = 0; i < TLB_SIZE; i++) {
                if(tlb[i].valid)
                    fprintf(out_fp, "%d - %d\n", tlb[i].logical, tlb[i].physical);
            }
            continue;  // Pula para a próxima linha
        }

        // Se não for um comando, trata como um endereço lógico
        int logical_address = atoi(line);
        total_addresses++;
        time_counter++;

        int offset = logical_address & OFFSET_MASK;
        int logical_page = (logical_address >> OFFSET_BITS) & 0xFF;
        int physical_frame = -1;

        // Procura na TLB
        for (int i = 0; i < TLB_SIZE; i++) {
            if(tlb[i].valid && tlb[i].logical == logical_page) {
                physical_frame = tlb[i].physical;
                tlb_hits++;
                if(strcmp(alg, "LRU") == 0)
                    frames[physical_frame].last_access_time = time_counter;
                break;
            }
        }

        // Se não encontrou na TLB, consulta a tabela de páginas
        if(physical_frame == -1) {
            physical_frame = pagetable[logical_page];
            if(physical_frame != -1 && strcmp(alg, "LRU") == 0)
                frames[physical_frame].last_access_time = time_counter;

            // Se a página não estiver em memória, ocorre um page fault
            if(physical_frame == -1) {
                page_faults++;
                if(next_free_frame < num_frames) {
                    physical_frame = next_free_frame;
                    next_free_frame++;
                } else {
                    int victim_frame = 0;
                    if(strcmp(alg, "FIFO") == 0) {
                        int min_load = frames[0].load_time;
                        for (int i = 1; i < num_frames; i++) {
                            if (frames[i].load_time < min_load) {
                                min_load = frames[i].load_time;
                                victim_frame = i;
                            }
                        }
                    } else if(strcmp(alg, "LRU") == 0) {
                        int min_access = frames[0].last_access_time;
                        for (int i = 1; i < num_frames; i++) {
                            if (frames[i].last_access_time < min_access) {
                                min_access = frames[i].last_access_time;
                                victim_frame = i;
                            }
                        }
                    }
                    physical_frame = victim_frame;
                    int evicted_logical = frames[victim_frame].logical_page;
                    if(evicted_logical != -1) {
                        pagetable[evicted_logical] = -1;
                        for (int i = 0; i < TLB_SIZE; i++) {
                            if(tlb[i].valid && tlb[i].logical == evicted_logical)
                                tlb[i].valid = 0;
                        }
                    }
                }
                // Carrega a página do backing store
                memcpy(physical_memory + physical_frame * PAGE_SIZE,
                       backing + logical_page * PAGE_SIZE,
                       PAGE_SIZE);
                pagetable[logical_page] = physical_frame;
                frames[physical_frame].logical_page = logical_page;
                frames[physical_frame].load_time = time_counter;
                frames[physical_frame].last_access_time = time_counter;
            }
            // Atualiza a TLB (substituição FIFO circular)
            tlb[tlb_next].logical = logical_page;
            tlb[tlb_next].physical = physical_frame;
            tlb[tlb_next].valid = 1;
            tlb_next = (tlb_next + 1) % TLB_SIZE;
        }

        int physical_address = (physical_frame << OFFSET_BITS) | offset;
        int value = physical_memory[physical_frame * PAGE_SIZE + offset];

        fprintf(out_fp, "Endereço Virtual: %d  Endereço Físico: %d Conteúdo: %d\n",
                logical_address, physical_address, value);
    }

    double page_fault_rate = (double) page_faults / total_addresses;
    double tlb_hit_rate = (double) tlb_hits / total_addresses;

    fprintf(out_fp, "Número de Endereços Traduzidos = %d\n", total_addresses);
    fprintf(out_fp, "Page Faults = %d\n", page_faults);
    fprintf(out_fp, "Taxa de Page Fault = %.3f\n", page_fault_rate);
    fprintf(out_fp, "TLB Hits = %d\n", tlb_hits);
    fprintf(out_fp, "Taxa de TLB Hit = %.3f\n", tlb_hit_rate);

    printf("Número de Endereços Traduzidos = %d\n", total_addresses);
    printf("Page Faults = %d\n", page_faults);
    printf("Taxa de Page Fault = %.3f\n", page_fault_rate);
    printf("TLB Hits = %d\n", tlb_hits);
    printf("Taxa de TLB Hit = %.3f\n", tlb_hit_rate);

    fclose(addr_fp);
    fclose(out_fp);
    free(physical_memory);
    free(frames);
    munmap(backing, backing_size);
    close(backing_fd);

    return 0;
}
