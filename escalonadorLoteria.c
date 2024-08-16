#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

typedef struct st_processo { // Define a estrutura que terá o processo
    char name[20];
    int id;
    int clock;
    int bilhetes;
    int tempo_exec;
} processo;

processo *lista_processos = NULL; //Cria uma lista de processos vazia
int num_processos = 0;
int num_bilhetes = 0;
int sum_clocks = 0;
int tempo = 0;
pthread_mutex_t lock; //Cria uma variável que funcionará como uma flag entre as threads

int contar_processos(void){ //Conta quantos processos tem no arquivo de entrada
    FILE *fp = fopen("entradaEscalonador1.txt", "r");
    int count = 0;
    char ch;

    while ((ch = getc(fp)) != EOF) {
        if (ch == '\n') {
            count++;
        }
    }
    fclose(fp);
    return count - 1;
}

int sorteio(int num_bilhetes) {//Realiza o sorteio 
    int sorteado = rand() % (num_bilhetes + 1);
    return sorteado;
}

void* adicionar_processo(void* arg) {
    /*É a função da thread que ficará sempre aguardando o usuário digitar
    um novo processo enquanto os outros estão execuntado*/
    printf("\n");
    printf("Caso queira adicionar um novo processo digite nesse formato: nome|id|clock|bilhetes \n");
    char linha[100];
    char nome[50];
    int clock, id, bilhetes;
    while (1) {
        fgets(linha, sizeof(linha), stdin); //Lê a entrada do usuário

        pthread_mutex_lock(&lock);
        num_processos++;
        lista_processos = realloc(lista_processos, num_processos * sizeof(processo));
        int resultado = sscanf(linha, "%[^|]|%d|%d|%d", nome, &id, &clock, &bilhetes);
        if (resultado == 4) {//Verifica se o usuário digitou 4 parâmetros
            strcpy(lista_processos[num_processos - 1].name, nome);
            lista_processos[num_processos - 1].id = id;
            lista_processos[num_processos - 1].clock = clock;
            lista_processos[num_processos - 1].bilhetes = bilhetes;
            lista_processos[num_processos - 1].tempo_exec = tempo;

            num_bilhetes += lista_processos[num_processos - 1].bilhetes;
            sum_clocks += lista_processos[num_processos - 1].clock;

            printf("Novo processo adicionado: %s\n", lista_processos[num_processos - 1].name);
            printf("Id: %d \n", lista_processos[num_processos - 1].id);
            printf("Clock: %d \n", lista_processos[num_processos - 1].clock);
            printf("Bilhetes: %d \n\n", lista_processos[num_processos - 1].bilhetes);
        }
        else{
            int resultado = sscanf(linha, "%s", nome);

                if(resultado == 1 && (strcmp(nome, "s") == 0 || strcmp(nome, "S") == 0)){//Verifica se o usuário quer encerrar o programa
                    lista_processos[num_processos-1].id = -1;
                    break;
                }
        }


        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* executar_processos(void* arg) {
    /*É a função que executa os processos na CPU*/

    printf("\n----------Inicio do escalonamento Loteria----------\n");
    printf("\n");
    int clock_cpu = *((int*)arg);

    while (1) {
        if (sum_clocks > 0 )
        {
            int num_sorteado = sorteio(num_bilhetes);
            int aux = 0;

            pthread_mutex_lock(&lock);
            for (int i = 0; i < num_processos; i++) {
                if (num_sorteado < aux + lista_processos[i].bilhetes) {
                    if (lista_processos[i].clock > 0) {//Verifica se o processo ainda precisa ser executado
                        printf("O processo %d está na CPU \n", lista_processos[i].id);
                        lista_processos[i].clock -= clock_cpu;
                        sum_clocks -= clock_cpu;
                        tempo += clock_cpu;
                        printf("Resta %d de clock no processo %d \n", lista_processos[i].clock, lista_processos[i].id);
                        printf("O processo %d saiu da CPU \n", lista_processos[i].id);
                        if(lista_processos[i].clock <= 0){ //Verifica se acabou o tempo de execução do processo
                            lista_processos[i].tempo_exec = tempo - lista_processos[i].tempo_exec;
                            num_bilhetes -= lista_processos[i].bilhetes;
                            lista_processos[i].bilhetes = 0;
                            printf("Processo %d finalizado \n", lista_processos[i].id);
                        }
                        printf("\n");
                        break;
                    }
                } else {
                    aux += lista_processos[i].bilhetes;
                }
            }
            pthread_mutex_unlock(&lock);

            sleep(1);       
        }

        else{
            if(lista_processos[num_processos -1].id == -1){// Verifica se o usuário quer encerrar o programa
                printf("Programa encerrado \n");
                break;
            }
            printf("Todos os processos foram encerrados\n");
            printf("\n");
            printf("Caso queira adicionar um novo processo digite nesse formato: nome|id|clock|bilhetes \n");
            printf("\n");
            printf("Caso queira encerrar digite (s) \n");
            sleep(5);
        }
        
    }

    /*Escreve no arquivo de saída a latência dos processos*/
    FILE *fp = fopen("SaidaLoteria.txt", "w");
    fprintf(fp,"ID | LATÊNCIA\n");
    for(int i = 0; i < num_processos; i++){
        if (lista_processos[i].id >= 0)
        {
            fprintf(fp,"%d | ", lista_processos[i].id);
            fprintf(fp,"       %d \n", lista_processos[i].tempo_exec);
        }
        
    }

    fclose(fp);
    return NULL;
}

void escalonadorLoteria() {

    srand(time(NULL));
    FILE *fp = fopen("entradaEscalonador1.txt", "r");
    char ch, *processos, *alg;
    int count = 0, clock_cpu;

    char linha[100];

    pthread_mutex_init(&lock, NULL);

    while (fgets(linha, sizeof(linha), fp)) { //Verifica o algoritmo e o clock da cpu
        alg = strtok(linha, "|");
        clock_cpu = atoi(strtok(NULL, "|"));
        break;
    }

    num_processos = contar_processos();

    lista_processos = malloc(num_processos * sizeof(processo)); // Cria uma lista com a quantidade de processos do arquivo de entrada

    while (fgets(linha, sizeof(linha), fp)) {//Adiciona os processos na lista
        strcpy(lista_processos[count].name, strtok(linha, "|"));
        lista_processos[count].id = atoi(strtok(NULL, "|"));
        lista_processos[count].clock = atoi(strtok(NULL, "|"));
        lista_processos[count].bilhetes = atoi(strtok(NULL, "|"));
        lista_processos[count].tempo_exec = 0;
        count++;
    }

    for (int i = 0; i < num_processos; i++) { //Calcula a quantidade de bilhetes e o clock total
        num_bilhetes += lista_processos[i].bilhetes;
        sum_clocks += lista_processos[i].clock;
    }

    pthread_t thread_exec, thread_add;

    pthread_create(&thread_add, NULL, adicionar_processo, NULL); //Cria a thread que adiciona processos

    pthread_create(&thread_exec, NULL, executar_processos, &clock_cpu); //Cria a thread que executa os processos

    pthread_join(thread_exec, NULL);//Espera a finalizaçao da thread de execução

    pthread_cancel(thread_add); //Finaliza a thread de adicionar processos

    free(lista_processos); //Limpa a memória
    pthread_mutex_destroy(&lock); //Exclui a variável de flag
    fclose(fp); //Fecha o arquivo
}
