#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

typedef struct st_processo
{
    char name[20];
    int id;
    int clock;
    int bilhetes;
}processo;

int contar_processos(void){
    FILE *fp = fopen("entradaEscalonador.txt", "r");
    int count = 0;
    char ch;

    while ((ch = getc(fp)) != EOF)
    {
        if(ch == '\n'){
            count++;
        }
    }
    fclose(fp);
    return count-1;
}

int sorteio(int num_bilhetes){
    int sorteado = rand() % (num_bilhetes + 1);
    printf("%d \n", sorteado);
    return sorteado;
}

int main()
{
    srand(time(NULL));
    FILE *fp = fopen("entradaEscalonador.txt", "r");
    char ch, *processos, *alg;
    int num_processos=0,count=0, num_bilhetes = 0, clock_cpu;

    char linha[100];

    while (fgets(linha, sizeof(linha), fp)) { //Mostra qual o algoritmo e o clock
        alg = strtok(linha, "|");
        printf("O algoritmo é: %s\n", alg);
        clock_cpu = atoi(strtok(NULL, "|"));
        break;
    }

    printf("A fração da CPU será: %d\n", clock_cpu);

    num_processos = contar_processos(); //Conta quantos processos existem

    processo lista_processos[num_processos]; //Cria uma lista com as informações dos processos

    while (fgets(linha, sizeof(linha), fp)){
        strcpy(lista_processos[count].name,strtok(linha, "|"));
        lista_processos[count].id = atoi(strtok(NULL, "|"));
        lista_processos[count].clock = atoi(strtok(NULL, "|"));
        lista_processos[count].bilhetes = atoi(strtok(NULL, "|"));
        printf("Processo: %s \n", lista_processos[count].name);
        printf("Id: %d \n", lista_processos[count].id);
        printf("Clock: %d \n", lista_processos[count].clock);
        printf("Bilhetes: %d \n\n", lista_processos[count].bilhetes);
        count++;

    };

    for (int i = 0; i < num_processos; i++)
    {
        num_bilhetes += lista_processos[i].bilhetes;
    }

    printf("O número total de bilhetes do sorteio será: %d \n", num_bilhetes);
    
    for(int i = 0; i < 10; i++){
        sorteio(num_bilhetes);
    };

    fclose(fp);
    return 0;
}
