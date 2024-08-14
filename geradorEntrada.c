#include <stdio.h>
#include <stdlib.h> 
#include <time.h> 



int main() {
    int alg, clock, nProcess;
    const char* algChosen;
    char nOfProcess[12];   
    char processITP[1024] = "processo"; // processo, ID, Time and Priority

    printf("Bem-vindo ao gerador de arquivos de entrada para o escalonador!\n");
    printf("Escolha o algoritmo: 1: alternancia circular, 2: prioridade, 3: loteria, 4: CFS\n");
    scanf("%d",&alg);
    
    printf("Informe a fracao de CPU que cada processo tera direito por vez\n");
    scanf("%d",&clock);

    printf("Informe o numero de processos a serem criados\n");
    scanf("%d",&nProcess);
    // Converte o int para string
    sprintf(nOfProcess, "%d", nProcess);

    switch (alg) {
        case 1: algChosen = "alternanciaCircular"; break;
        case 2: algChosen = "prioridade"; break;
        case 3: algChosen = "loteria"; break;
        case 4: algChosen = "cfs"; break;
        default:
            printf("O algoritmo informado nao existe\n");
            return 1;
    }


    const char *dirFile= "./entradaEscalonador1.txt"; // caminho relativo

    FILE *out = fopen(dirFile, "w");
    if (out == NULL) {
        perror("Erro ao criar o arquivo");
        return 1;
    }

    // Escreve o algoritmo escolhido e numero de processos
    fprintf(out, "%s|%d\n", algChosen, clock); // Escreve no documento Ex.: alternanciaCircular|5

    int time, priority;
    char intString[12], 
        timeString[12],
        priorityString[12]; 

    int min = clock, max = 99;
	for(int i = 0; i < nProcess; i++){
        sprintf(intString, "%d", i);

        time = (rand() % ((max / clock) - (min / clock) + 1) + (min / clock)) * clock;
        sprintf(timeString, "%d", time);

        priority = (rand() % (max - min + 1)) + min; 
        sprintf(priorityString, "%d", priority);

        fprintf(out, "processo-%s|%s|%s|%s\n", intString, intString, timeString, priorityString); // Escreve no documento Ex.: processo-0|0|5|41
    }

        processITP[0] = '\0';

    fclose(out);

    return 0;
}
