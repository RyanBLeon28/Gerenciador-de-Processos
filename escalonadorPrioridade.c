#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>


typedef struct{
    char nome_processo[50];
    int id,
        prioridade,
        tempo_execucao,
        latencia,
        latenciaAtual;
}DadosProcessos;

typedef struct {
    int clock_cpu;
        // currentClock;
} ClockCPU;

int clockAtual = 0;

ClockCPU *cpuClockPR;

pthread_mutex_t mutex_prioridade_inserindo_processo;
DadosProcessos *lista_processosPR = NULL;
int iterador;


int conta_processos(){   
    char linha[50];
    FILE *arquivo_1 = fopen("entradaEscalonador1.txt", "r");

    int numero_processos = 0, controlador = 0;
 
    while (fgets(linha, sizeof(linha), arquivo_1) != NULL){
        if (controlador == 0){ 
            controlador++;
        }
        else{
           numero_processos++;
        }
    }   

    fclose(arquivo_1);
    return numero_processos;
    
}  

void *recebe_novos_processos(void* arg){

    char resposta[10], linha[50];
    int teste = 0;
    
    iterador = *(int*)arg;

    printf("Deseja um novo processo? Caso queira adicionar um novo processo siga o padrão: Nome do processo|Id|Tempo necessário|Prioridade.\n");


    while (true){
        fgets(linha, sizeof(linha), stdin);
        pthread_mutex_lock(&mutex_prioridade_inserindo_processo);   

        iterador++;

        lista_processosPR = realloc(lista_processosPR, iterador * sizeof(DadosProcessos));

        strcpy(lista_processosPR[iterador - 1].nome_processo, strtok(linha, "|"));
        lista_processosPR[iterador - 1].id = atoi(strtok(NULL, "|"));
        lista_processosPR[iterador - 1].tempo_execucao = atoi(strtok(NULL, "|"));
        lista_processosPR[iterador - 1].prioridade = atoi(strtok(NULL, "|"));
        lista_processosPR[iterador - 1].latenciaAtual = clockAtual;

        pthread_mutex_unlock(&mutex_prioridade_inserindo_processo);    
    }
        
}

void *executando_processos(void* arg){
    
    int latencia = 0, id_processo_anterior;

    while(true){
        clockAtual += cpuClockPR->clock_cpu;

        int maior_prioridade = 0, j = 0, posicao = 0,  clock = *(int*)arg;;
        while(j < iterador){
            if (maior_prioridade < lista_processosPR[j].prioridade && lista_processosPR[j].tempo_execucao > 0){
                maior_prioridade = lista_processosPR[j].prioridade;
                posicao = j;
                j++;
            }
            else{
                j++;
            }    
        }

        if (maior_prioridade > 0){
            pthread_mutex_lock(&mutex_prioridade_inserindo_processo);


            if (lista_processosPR[posicao].tempo_execucao - clock >= 0){
                lista_processosPR[posicao].tempo_execucao = lista_processosPR[posicao].tempo_execucao - clock;
            }
            else{
                clock = lista_processosPR[posicao].tempo_execucao;
                lista_processosPR[posicao].tempo_execucao = 0;
            }

            if (lista_processosPR[posicao].id == id_processo_anterior){
                lista_processosPR[posicao].latencia = lista_processosPR[posicao].latencia + clock;

            }
            else{
                lista_processosPR[posicao].latencia = lista_processosPR[posicao].latencia + latencia + clock;
                lista_processosPR[posicao].latencia += + latencia + clockAtual;
            }

            latencia = latencia + clock;
            // latencia = latencia + clock;
            // process->latency + currentClock;
                         
            if(lista_processosPR[posicao].tempo_execucao <= 0){ //Processo encerrou
                int deltaLatencia = lista_processosPR[posicao].latencia - lista_processosPR[posicao].latenciaAtual;        
                lista_processosPR[posicao].latencia = deltaLatencia; 
                printf("latecia final %d \n",lista_processosPR[posicao].latencia); 
                lista_processosPR[posicao].prioridade = 0;          
            }

            //printf("Nome do processo:%s\nId do processo:%d\nTempo de execução do processo:%d\nPrioridade do processo:%d\n", lista_processosPR[posicao].nome_processo, lista_processosPR[posicao].id, lista_processosPR[posicao].tempo_execucao, lista_processosPR[posicao].prioridade);

            printf("Nome do processo:%s\nTempo de execução do processo:%d\nLatência do processo:%d\n", lista_processosPR[posicao].nome_processo, lista_processosPR[posicao].tempo_execucao, lista_processosPR[posicao].latencia);

            pthread_mutex_unlock(&mutex_prioridade_inserindo_processo); 

            id_processo_anterior = lista_processosPR[posicao].id;

            printf("\n");
            sleep(1);
        }
        else{
            break;
        }
    }
}

void escalonadorPrioridade (){
    char linha[50];
    int controlador = 0, numero_processos = 0, i = 0;
    int clock;

    numero_processos = conta_processos();
    lista_processosPR = malloc(numero_processos * sizeof(DadosProcessos));

    FILE *arquivo_2 = fopen("entradaEscalonador1.txt", "r");

    while (fgets(linha, sizeof(linha), arquivo_2) != NULL){
        if (controlador == 0){
            strtok(linha, "|");
            clock = atoi(strtok(NULL, "|"));
            controlador++;
            
            cpuClockPR = malloc(sizeof(ClockCPU));  
            cpuClockPR->clock_cpu = clock; 
        }


        else{            
            strcpy(lista_processosPR[i].nome_processo, strtok(linha, "|"));
            lista_processosPR[i].id = atoi(strtok(NULL, "|"));
            lista_processosPR[i].tempo_execucao = atoi(strtok(NULL, "|"));
            lista_processosPR[i].prioridade = atoi(strtok(NULL, "|"));
            lista_processosPR[i].latencia = 0;
            i++;
        }
    }
    fclose(arquivo_2);

    pthread_t executando_processo, lendo_novo_processo;
    pthread_mutex_init(&mutex_prioridade_inserindo_processo, NULL);

    pthread_create(&lendo_novo_processo, NULL, &recebe_novos_processos, &numero_processos);
    pthread_create(&executando_processo, NULL, &executando_processos, &clock);

    pthread_join(executando_processo,NULL);

    pthread_cancel(lendo_novo_processo);

    pthread_mutex_destroy(&mutex_prioridade_inserindo_processo);
    free(lista_processosPR);
}

