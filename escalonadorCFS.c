#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <glib.h>       // Biblioteca para Red-Black Tree
#include "escalonadorCFS.h"
#include <stdbool.h>



typedef struct {
    int clock_cpu;
} ClockCPU;

typedef struct {
    char name[20];
    int id,
        clock,
        priority,
        isAvailable;
} Process;

typedef struct {
    GTree *tree;
    ClockCPU *clock;
    GList *processes_to_remove;
    gboolean *stop_iteration;
} ExecData;

bool isProcessEmpty(Process p) {
    return p.isAvailable == 0 && p.id == 0 && p.clock == 0 && p.priority == 0;
}

// Função de comparação usada pelo GTree
gint compare_ints(gconstpointer a, gconstpointer b) {
    return (*(const int *)a - *(const int *)b);
}

// Função para imprimir chave/valor
gboolean print_key_value(gpointer key, gpointer value, gpointer user_data) {
    Process *process = value;
    printf("Clock: %d, ID: %d, Nome: %s, Clock: %d, Prioridade: %d\n",
           *(int *)key, process->id, process->name, process->clock, process->priority);
    return FALSE;
}

void removeProcess(ExecData *data) {
    GList *iter = data->processes_to_remove;
    while (iter != NULL) {
        int *key = (int *)iter->data; // Chave a ser removida
        Process *process = g_tree_lookup(data->tree, key); // Buscar o processo correspondente
        if (process != NULL) {
            g_tree_remove(data->tree, key);
            printf("\nProcesso ID: %d encerrou\n", process->id);
            free(process); // Liberar o processo
        }
        free(key); // Liberar a chave alocada
        iter = iter->next; // Avançar para o próximo item da lista
    }
    g_list_free(data->processes_to_remove);
    data->processes_to_remove = NULL;
}

// Funcao para executar o processo dentro da CPU e remover o tempo utilizado
gboolean execProcess(gpointer key, gpointer value, gpointer user_data) {
    ExecData *data = (ExecData *)user_data;
    GTree *tree = data->tree;
    ClockCPU *clock = data->clock;
    gboolean *stop_iteration = data->stop_iteration;

    Process *process = value;
    printf("\n Processo %d entrou na CPU \n", process->id);
    
    int restTime = process->clock - clock->clock_cpu;
    printf("Tempo anterior: %d, Tempo restante: %d\n", process->clock, restTime);

    process->clock = restTime;
    int *new_key = malloc(sizeof(int));
    *new_key = process->clock;

    if (process->clock <= 0) {
        // Fim do processo, então é removido da árvore
        data->processes_to_remove = g_list_prepend(data->processes_to_remove, key);
        *stop_iteration = TRUE; // Solicita a parada da iteração
        return TRUE; // Interrompe a iteração atual
    } else {
        // Atualiza a chave removendo a antiga e colocando uma nova no mesmo lugar
        g_tree_remove(tree, key);
        free(key); // Liberar a chave antiga
        g_tree_insert(tree, new_key, process);
    }
    sleep(1);
    return FALSE;
}

// Função que encapsula o código do escalonador
void escalonadorCFS(Process process) {
    const char *filename = "entradaEscalonador1.txt";

    char *alg;
    int clock_cpu;
    char linha[100];

    // Cria a árvore Red-Black
    GTree *tree = g_tree_new(compare_ints);
    ClockCPU *clock = NULL;

    if (isProcessEmpty(process)) {
        printf("Process is empty.\n");
        
        FILE *fp = fopen(filename, "r");
        if (fp == NULL) {
            perror("Erro ao abrir o arquivo");
            exit(EXIT_FAILURE);
        }
        
        // Lê o algoritmo e o clock
        if (fgets(linha, sizeof(linha), fp)) {
            alg = strtok(linha, "|");
            clock_cpu = atoi(strtok(NULL, "|"));
        }

        printf("A fração da CPU será: %d\n", clock_cpu);
        clock = malloc(sizeof(ClockCPU));
        clock->clock_cpu = clock_cpu;

        // Lê os processos do arquivo
        while (fgets(linha, sizeof(linha), fp)) {
            Process *proc = malloc(sizeof(Process));
            strcpy(proc->name, strtok(linha, "|"));
            proc->id = atoi(strtok(NULL, "|"));
            proc->clock = atoi(strtok(NULL, "|"));
            proc->priority = atoi(strtok(NULL, "\n"));

            int *clock_copy = malloc(sizeof(int));
            *clock_copy = proc->clock;

            g_tree_insert(tree, clock_copy, proc);
        }

        // Percorrendo e imprimindo a árvore após toda a inserção
        printf("\n Lista de processos\n");
        g_tree_foreach(tree, print_key_value, NULL);

        fclose(fp); // Certifique-se de fechar o arquivo
    }
    
    // Executando o escalonamento CFS
    printf("\n----------Inicio do escalonamento CFS----------\n");
    while (TRUE) {
        gboolean stop_iteration = FALSE;
        ExecData data = {tree, clock, NULL, &stop_iteration};

        pthread_mutex_lock(&mutex);
        if (process.isAvailable) {
            int *clock_copy = malloc(sizeof(int));
            *clock_copy = process.clock;

            Process *new_process = malloc(sizeof(Process));
            *new_process = process;

            g_tree_insert(tree, clock_copy, new_process);

            printf("Novo processo adicionado - ID: %d, Clock: %d, Prioridade: %d\n", 
                   new_process->id, new_process->clock, new_process->priority);

            process.isAvailable = 0;
        }
        pthread_mutex_unlock(&mutex);

        g_tree_foreach(tree, execProcess, &data);    // Processo entra na CPU

        removeProcess(&data);                        // Remove os processos que acabaram

        if (stop_iteration) {
            continue; // volta pro inicio do loop
        }

        if (g_tree_nnodes(tree) == 0) {
            printf("CPU vazia.\n");
        }
        sleep(1);
    }

    // Liberando a árvore, a estrutura e o arquivo
    g_tree_destroy(tree);
    free(clock);
}