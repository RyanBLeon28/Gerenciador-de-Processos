#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

typedef struct st_processo {
    char name[20];
    int id;
    int clock;
    int bilhetes;
    int tempo_exec;
} processo;

processo *lista_processos = NULL;
int num_processos = 0;
int num_bilhetes = 0;
int sum_clocks = 0;
int tempo = 0;
pthread_mutex_t lock;

int contar_processos(void){
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

int sorteio(int num_bilhetes) {
    int sorteado = rand() % (num_bilhetes + 1);
    return sorteado;
}

void* adicionar_processo(void* arg) {
    printf("\n");
    printf("Caso queira adicionar um novo processo digite nesse formato: nome|id|clock|bilhetes ");
    printf("\n");
    char linha[100];
    char nome[50];
    int clock, id, bilhetes;
    while (1) {
        fgets(linha, sizeof(linha), stdin);

        pthread_mutex_lock(&lock);
        int resultado = sscanf(linha, "processo-%[^|]|%d|%d|%d", nome, &id, &clock, &bilhetes);
        if (resultado == 4) { 
            num_processos++;
            lista_processos = realloc(lista_processos, num_processos * sizeof(processo));
            
            strcpy(lista_processos[num_processos - 1].name, nome);
            lista_processos[num_processos - 1].id = atoi(strtok(NULL, "|"));
            lista_processos[num_processos - 1].clock = atoi(strtok(NULL, "|"));
            lista_processos[num_processos - 1].bilhetes = atoi(strtok(NULL, "|"));
            lista_processos[num_processos - 1].tempo_exec = tempo;

            num_bilhetes += lista_processos[num_processos - 1].bilhetes;
            sum_clocks += lista_processos[num_processos - 1].clock;

            printf("Novo processo adicionado: %s\n", lista_processos[num_processos - 1].name);
            printf("Id: %d \n", lista_processos[num_processos - 1].id);
            printf("Clock: %d \n", lista_processos[num_processos - 1].clock);
            printf("Bilhetes: %d \n\n", lista_processos[num_processos - 1].bilhetes);
        }
        else{
            int resultado
        }


        pthread_mutex_unlock(&lock);
    }
    return NULL;
}

void* executar_processos(void* arg) {
    int clock_cpu = *((int*)arg);

    while (TRUE) {
        if (sum_clocks > 0 )
        {
            int num_sorteado = sorteio(num_bilhetes);
            int aux = 0;

            pthread_mutex_lock(&lock);
            for (int i = 0; i < num_processos; i++) {
                if (num_sorteado < aux + lista_processos[i].bilhetes) {
                    if (lista_processos[i].clock > 0) {
                        //printf("O número sorteado foi: %d \n", num_sorteado);
                        printf("O processo %d está na CPU \n", lista_processos[i].id);
                        lista_processos[i].clock -= clock_cpu;
                        sum_clocks -= clock_cpu;
                        tempo += clock_cpu;
                        printf("Resta %d de clock no processo %d \n", lista_processos[i].clock, lista_processos[i].id);
                        if(lista_processos[i].clock <= 0){
                            lista_processos[i].tempo_exec = tempo - lista_processos[i].tempo_exec;
                            num_bilhetes -= lista_processos[i].bilhetes;
                            lista_processos[i].bilhetes = 0;
                            // printf("%d \n\n", num_bilhetes);
                            printf("Processo %d finalizado \n", lista_processos[i].id);
                        }
                        // printf("O processo %d saiu da CPU \n", lista_processos[i].id);
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
            printf("Todos os processos foram encerrados\n");

        }
        
    }

    FILE *fp = fopen("SaidaLoteria.txt", "w");
    fprintf(fp,"ID | TEMPO DE EXECUÇÃO\n");
    for(int i = 0; i < num_processos; i++){
        fprintf(fp,"%d | ", lista_processos[i].id);
        fprintf(fp,"       %d \n", lista_processos[i].tempo_exec);
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

    while (fgets(linha, sizeof(linha), fp)) { // Mostra qual o algoritmo e o clock
        alg = strtok(linha, "|");
        //printf("O algoritmo é: %s\n", alg);
        clock_cpu = atoi(strtok(NULL, "|"));
        break;
    }

    // printf("A fração da CPU será: %d\n", clock_cpu);

    num_processos = contar_processos(); // Conta quantos processos existem

    lista_processos = malloc(num_processos * sizeof(processo)); // Cria uma lista com as informações dos processos

    while (fgets(linha, sizeof(linha), fp)) {
        strcpy(lista_processos[count].name, strtok(linha, "|"));
        lista_processos[count].id = atoi(strtok(NULL, "|"));
        lista_processos[count].clock = atoi(strtok(NULL, "|"));
        lista_processos[count].bilhetes = atoi(strtok(NULL, "|"));
        lista_processos[count].tempo_exec = 0;
        // printf("Processo: %s \n", lista_processos[count].name);
        // printf("Id: %d \n", lista_processos[count].id);
        // printf("Clock: %d \n", lista_processos[count].clock);
        // printf("Bilhetes: %d \n\n", lista_processos[count].bilhetes);
        count++;
    }

    for (int i = 0; i < num_processos; i++) {
        num_bilhetes += lista_processos[i].bilhetes;
        sum_clocks += lista_processos[i].clock;
    }

    //printf("O número total de bilhetes do sorteio será: %d \n", num_bilhetes);
    //printf("O número total de clocks será: %d \n", sum_clocks);

    pthread_t thread_exec, thread_add;

    pthread_create(&thread_add, NULL, adicionar_processo, NULL);
    // Criando a thread que executa os processos
    pthread_create(&thread_exec, NULL, executar_processos, &clock_cpu);

    // Criando a thread que permite adicionar novos processos

    // Aguardando a thread de execução dos processos terminar
    pthread_join(thread_exec, NULL);

    // Cancelando a thread de adição de processos, pois o loop principal acabou
    pthread_cancel(thread_add);

    free(lista_processos);
    pthread_mutex_destroy(&lock);
    fclose(fp);
}
