#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <glib.h>       // Biblioteca para Red-Black Tree
#include "escalonadorCFS.h"
#include <stdbool.h>

// Renomeie as variáveis globais
GTree *tree;

typedef struct {
    int clock_cpu;
} ClockCPU;

ClockCPU *cpuClock;
int currentClock = 0;

typedef struct {
    char name[20];
    int id,
        clock,
        priority,
        latency,
        currentLatency;
} Process;

pthread_mutex_t lockCFS;

typedef struct {
    GTree *tree;
    ClockCPU *cpuClock;  // Atualizado para o novo nome
    GList *processes_to_remove;
    gboolean *stop_iteration;
} ExecData;

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
    FILE *fp = fopen("SaidaCFS.txt", "a");

    while (iter != NULL) {
        int *key = (int *)iter->data; // Chave a ser removida
        Process *process = g_tree_lookup(data->tree, key); // Buscar o processo correspondente

        int newLatency = process->latency - process->currentLatency;
        process->latency = newLatency;
        printf("\nLat final: %d - %d \n", process->latency, process->currentLatency);

        //escreve o valor da latencia antes de excluir o processo
        fprintf(fp,"%d | ", process->id);
        fprintf(fp,"    %d \n", process->latency);

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
    fclose(fp);
}

void* adicionar_processo_CFS(void* arg) {
    char linha[100];
    while (1) {
        printf("Digite um novo processo (name|id|clock|bilhetes): \n");
        fgets(linha, sizeof(linha), stdin);

        pthread_mutex_lock(&lockCFS);
        
        Process *process = malloc(sizeof(Process));
        strcpy(process->name, strtok(linha, "|"));
        process->id = atoi(strtok(NULL, "|"));
        process->clock = atoi(strtok(NULL, "|"));
        process->priority = atoi(strtok(NULL, "\n"));
        process->currentLatency = currentClock;
        
        int *clock_copy = malloc(sizeof(int));
        *clock_copy = process->clock;

        g_tree_insert(tree, clock_copy, process);

        printf("Novo processo adicionado: %s\n", process->name);
        printf("Id: %d \n", process->id);
        printf("Clock: %d \n", process->clock);
        printf("Bilhetes: %d \n\n", process->priority);

        pthread_mutex_unlock(&lockCFS);
    }
    return NULL;
}


// Funcao para executar o processo dentro da CPU e remover o tempo utilizado
gboolean execProcess(gpointer key, gpointer value, gpointer user_data) {
    currentClock += cpuClock->clock_cpu;
    //printf("CPU: %d\n",currentClock);

    ExecData *data = (ExecData *)user_data;
    GTree *tree = data->tree;
    ClockCPU *cpuClock = data->cpuClock;
    gboolean *stop_iteration = data->stop_iteration;

    Process *process = value;
    printf("\n Processo %d entrou na CPU \n", process->id);
    
    process->latency = process->latency + currentClock;
    int restTime = process->clock - cpuClock->clock_cpu;
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

void* executar_processos_CFS(void* arg) {
    printf("\n----------Inicio do escalonamento CFS----------\n");

    while (TRUE) {
        gboolean stop_iteration = FALSE;
        ExecData data = {tree, cpuClock, NULL, &stop_iteration};  // Atualizado

        pthread_mutex_lock(&lockCFS);
        g_tree_foreach(tree, execProcess, &data);
        pthread_mutex_unlock(&lockCFS);

        removeProcess(&data);

        if (stop_iteration) {
            continue;
        }

        if (g_tree_nnodes(tree) == 0) {
            printf("CPU vazia.\n");
            currentClock = 0;
        }
        sleep(1);
    }

    return NULL;
}


void escalonadorCFS() {
    FILE *out = fopen("SaidaCFS.txt", "w");
    fprintf(out,"ID | LATÊNCIA\n");
    fclose(out);


    pthread_mutex_init(&lockCFS, NULL);

    const char *filename = "entradaEscalonador1.txt";
    char *alg;
    int clock_cpu;
    char linha[100];
    tree = g_tree_new(compare_ints); 

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
    cpuClock = malloc(sizeof(ClockCPU));  // Atualizado
    cpuClock->clock_cpu = clock_cpu;  // Atualizado

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

    // Percorrendo e imprimindo a árvore após toda a inserção lida do arquivo txt
    printf("\n Lista de processos\n");
    g_tree_foreach(tree, print_key_value, NULL);   

    pthread_t thread_exec, thread_add;

    pthread_create(&thread_add, NULL, adicionar_processo_CFS, NULL);
    // Criando a thread que executa os processos
    pthread_create(&thread_exec, NULL, executar_processos_CFS, &clock_cpu);

    // Criando a thread que permite adicionar novos processos

    // Aguardando a thread de execução dos processos terminar
    pthread_join(thread_exec, NULL);

    // Cancelando a thread de adição de processos, pois o loop principal acabou
    pthread_cancel(thread_add);


    g_tree_destroy(tree);
    free(cpuClock);  // Atualizado
    fclose(fp);
}