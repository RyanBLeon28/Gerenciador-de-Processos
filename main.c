#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "escalonadorCFS.h"
// #include "escalonadorLoteria.h"
// #include "escalonadorPrioridade.h"
// #include "escalonadorAlternanciaCircular.h"

#define TRUE 1

// Estrutura para passar dados para as threads
typedef struct {
    const char* filename;
} ThreadData;

typedef struct {
    char name[20];
    int id,
        clock,
        priority,
        isAvailable;
} Process;

Process process;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void* callCFS(void* arg) {
    escalonadorCFS(process);

    while (TRUE) {
        pthread_mutex_lock(&mutex);
        printf("Consumer thread: Checking process state...\n");
        printf("Process state - ID: %d, Clock: %d, Priority: %d, Available: %d\n", 
                process.id, process.clock, process.priority, process.isAvailable);

        if (process.isAvailable) {

            printf("Consumer thread: Received Process - ID: %d, Clock: %d, Priority: %d\n", 
                    process.id, process.clock, process.priority);
            
            escalonadorCFS(process);

            process.isAvailable = 0; 
            printf("Consumer thread: Process handled, isAvailable set to 0\n");
        } 
        else {
            printf("Consumer thread: No new process available\n");
        }

        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

void* waitProcess(void* arg){
    int id, clock, priority;
    
    while(TRUE){
        printf("                                                 Digite o ID do novo processo: \n");
        scanf("%d",&id);
        printf("                                                 Digite o tempo do novo processo: \n");
        scanf("%d",&clock);
        printf("                                                 Digite a prioridade do novo processo: \n");
        scanf("%d",&priority);

        printf("                                                 ID: %d, Clock: %d, Prioridade: %d \n",id, clock, priority);
        
        pthread_mutex_lock(&mutex);
        process.id = id;
        process.clock = clock;
        process.priority = priority;
        process.isAvailable = 1;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main() {
    printf("Arquivo principal\n");

     process.isAvailable = 0;
    int nThreads = 2;
    pthread_t threads[nThreads];
    ThreadData thread_data[nThreads];


    const char *filename = "entradaEscalonador1.txt";
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }


    // Lê o algoritmo e o clock
    char *alg;
    int clock_cpu;
    char linha[100];
    if (fgets(linha, sizeof(linha), fp)) {
        alg = strtok(linha, "|");
        printf("O algoritmo é: %s\n", alg);
        clock_cpu = atoi(strtok(NULL, "|"));
    }
    
    if (strcmp(alg, "CFS") == 0) {
        pthread_create(&threads[0], NULL, callCFS, (void*)&thread_data[0]);
        
    } 
    // else if (strcmp(alg, "escalonadorLoteria") == 0) {
    //     escalonadorLoteria(filename);
    // } 
    // else if (strcmp(alg, "escalonadorAlternanciaCircular") == 0) {
    //     escalonadorAlternanciaCircular(filename);
    // } 
    // else if (strcmp(alg, "escalonadorPrioridade") == 0) {
    //     escalonadorPrioridade(filename);
    // } 
    else {
        printf("Algoritmo desconhecido: %s\n", alg);
    }

    pthread_create(&threads[1], NULL, waitProcess, (void*)&thread_data[1]);

   // Wait for threads to finish
    for (int i = 0; i < nThreads; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    return 0;
}