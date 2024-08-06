#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

typedef struct st_processo
{
    char name[20];
    int id;
    int clock;
    int bilhetes;
}processo;


int main()
{
    FILE *fp = fopen("entradaEscalonador1.txt", "r");
    char ch, *processos, *palavra;
    int int_processos, numero, count=0, num_bilhetes = 0;

    char linha[100];

    // Leia cada linha do arquivo e imprima na tela
    while (fgets(linha, sizeof(linha), fp)) {
        palavra = strtok(linha, "|");
        printf("O algoritmo é: %s\n", palavra);
        processos = strtok(NULL, "|");
        break;
    }

    int_processos = atoi(processos);
    printf("O número de processos é: %d\n", int_processos);

    processo lista_processos[int_processos];

    while (fgets(linha, sizeof(linha), fp)){
        strcpy(lista_processos[count].name,strtok(linha, "|"));
        lista_processos[count].id = atoi(strtok(NULL, "|"));
        lista_processos[count].clock = atoi(strtok(NULL, "|"));
        lista_processos[count].bilhetes = atoi(strtok(NULL, "|"));
        count++;
    };

    for (int i = 0; i < int_processos; i++)
    {
        num_bilhetes += lista_processos[i].bilhetes;
    }
    printf(" O número total de bilhetes do sorteio será: %d \n", num_bilhetes);
    

    fclose(fp);
    return 0;
}
