#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "escalonadorCFS.h"
#include "escalonadorLoteria.h"
#include "escalonadorPrioridade.h"
#include "escalonadorAlternanciaCircular.h"

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
    
    if (strcmp(alg, "cfs") == 0) {
        escalonadorCFS();
    } 
    else if (strcmp(alg, "loteria") == 0) {
        escalonadorLoteria();
    } 
    else if (strcmp(alg, "alternanciaCircular") == 0) {
        escalonadorAC();
    } 
    else if (strcmp(alg, "prioridade") == 0) {
        escalonadorPrioridade();
    } 
    else {
        printf("Algoritmo desconhecido: %s\n", alg);
    }

    return 0;
}