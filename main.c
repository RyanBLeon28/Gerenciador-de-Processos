#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "escalonadorCFS.h"
// #include "escalonadorLoteria.h"
// #include "escalonadorPrioridade.h"
// #include "escalonadorAlternanciaCircular.h"


// Estrutura para passar dados para as threads
typedef struct {
    const char* fileDir;
    int nOfFiles;
} ThreadData;

void waitProcess(){
    int id, clock, priority;
    printf("Digite o ID do novo processo: ");
    scanf("%d",&id);
    printf("Digite o tempo do novo processo: ");
    scanf("%d",&id);
    printf("Digite a prioridade do novo processo: ");
    scanf("%d",&id);
}

int main() {
    printf("Arquivo principal\n");

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
        escalonadorCFS(filename);
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
    //pthread_create(&threads[0], NULL, execProcess, (void*)&thread_data[0]);

    //pthread_create(&threads[1], NULL, waitProcess, (void*)&thread_data[1]);

    return 0;
}