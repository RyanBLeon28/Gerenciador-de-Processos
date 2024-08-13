#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include "escalonadorCFS.h"
#include "escalonadorLoteria.h"
// #include "escalonadorPrioridade.h"
// #include "escalonadorAlternanciaCircular.h"

#define TRUE 1


int main() {    
    printf("Arquivo principal\n");

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
        // pthread_create(&threads[0], NULL, callCFS, NULL);
    } 
    else if (strcmp(alg, "Loteria") == 0) {
        escalonadorLoteria();
    } 
    // else if (strcmp(alg, "escalonadorAlternanciaCircular") == 0) {
    //     escalonadorAlternanciaCircular(filename);
    // } 
    // else if (strcmp(alg, "escalonadorPrioridade") == 0) {
    //     escalonadorPrioridade(filename);
    // } 
    else {
        printf("Algoritmo desconhecido: %s\n", alg);
    }

    return 0;
}