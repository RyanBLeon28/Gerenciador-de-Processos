#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

//Dados dos processos
typedef struct{
    char nome_processo[50];
    int id,
        prioridade,
        tempo_execucao,
        latencia;
}DadosProcessos;

pthread_mutex_t mutex_prioridade; //Criando um mutex
DadosProcessos *lista_processosPR = NULL; //Criando a lista onde os processos vão ser armazenados.
int iterador; //Iterador para contar o número de processos.


//Função utilizada para contar os processos. Retorna o número de linhas (a partir da segunda) do arquivo txt. 
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


//Função para criar um arquivo txt onde estarão armazenados o Id e a Latência de cada processo.
void criando_arquivo(){
    int i = 0;
    FILE* arquivo_3 = fopen("SaidaPrioridade.txt", "w");
    fprintf(arquivo_3, "ID | LATÊNCIA\n");

    while (i < iterador){
        if (lista_processosPR[i].id >= 0){
            fprintf(arquivo_3, "%d | ", lista_processosPR[i].id);
            fprintf(arquivo_3, "%d\n", lista_processosPR[i].latencia);
        }
        i++;
    }
}

//Função que recebe novos processos e armazena numa lista. Durante o procedimento de adicionar um novo processo ele "trava" a função executando_processos usando um mutex.
void *recebe_novos_processos(void* arg){

    char resposta, linha[50];
    int teste = 0;
    
    iterador = *(int*)arg;
    printf("Caso deseja inserir um novo processo siga o padrão: nome do processo|Id|Tempo de execução|Prioridade\n");

    while (true){
        fgets(linha, sizeof(linha), stdin);

        pthread_mutex_lock(&mutex_prioridade);  

        iterador++;

        lista_processosPR = realloc(lista_processosPR, iterador * sizeof(DadosProcessos));
        
        char nome[50];
        int id , tempo, prioridade;
        int result = sscanf(linha, "%[^|]|%d|%d|%d", nome, &id, &tempo, &prioridade); 
        
        if (result == 4) { 
            strcpy(lista_processosPR[iterador - 1].nome_processo, nome);
            lista_processosPR[iterador - 1].id = id;
            lista_processosPR[iterador - 1].tempo_execucao = tempo;
            lista_processosPR[iterador - 1].prioridade = prioridade;
            lista_processosPR[iterador - 1].latencia = 0;

            printf("Novo processo adicionado: %s\n", lista_processosPR[iterador - 1].nome_processo);
            printf("Id: %d \n", lista_processosPR[iterador - 1].id);
            printf("Clock: %d \n", lista_processosPR[iterador - 1].tempo_execucao);
            printf("Prioridade: %d \n\n", lista_processosPR[iterador - 1].prioridade);
        
        }
        else {
            int result = sscanf(linha, "%s", nome);

            if(result == 1 && strcmp(nome, "s") == 0){
                lista_processosPR[iterador - 1].id = -1;
                //printf("Thread Adiconar encerrou \n");
                break;
            }
        }

        pthread_mutex_unlock(&mutex_prioridade);
    }
}
        
//Função que recebe a lista de processos e executa-os. Durante o procedimento de executar um novo processo ele "trava" a função recebe_novos_processos usando um mutex.
void *executando_processos(void* arg){
    
    int latencia = 0, id_processo_anterior;

    while(true){
        int maior_prioridade = 0, j = 0, k = 0, posicao = 0,  clock = *(int*)arg;;
        while(j < iterador){
            if (maior_prioridade < lista_processosPR[j].prioridade && lista_processosPR[j].tempo_execucao > 0){
                maior_prioridade = lista_processosPR[j].prioridade;
                posicao = j;
            }
            j++;     
        }

        if (maior_prioridade > 0){
            pthread_mutex_lock(&mutex_prioridade);

            if (lista_processosPR[posicao].tempo_execucao - clock >= 0){
                lista_processosPR[posicao].tempo_execucao = lista_processosPR[posicao].tempo_execucao - clock;
            }
            else{
                clock = lista_processosPR[posicao].tempo_execucao;
                lista_processosPR[posicao].tempo_execucao = 0;
            }

            while(k < iterador){
                if (lista_processosPR[k].prioridade > 0){
                lista_processosPR[k].latencia += clock;
                }
                k++;
            }
         
            if(lista_processosPR[posicao].tempo_execucao <= 0){ //Processo encerrou

                lista_processosPR[posicao].prioridade = 0;          
            }

            printf("Id do processo:%d\nTempo de execucao do processo:%d\nLatencia do processo:%d\n", 
            lista_processosPR[posicao].id, 
            lista_processosPR[posicao].tempo_execucao, 
            lista_processosPR[posicao].latencia);

            pthread_mutex_unlock(&mutex_prioridade); 
            

            printf("\n");
            sleep(1);
        }
        else{
            if ( lista_processosPR[iterador-1].id == -1 ){
                //printf("Thread executar encerrou \n");
                break;
            }
            printf("Todos os processos foram executados. Deseja encerrar? S \n");
            sleep(3);
        }
    }
}

//Fução main que rebece que inicia as threads, mutex, recebe os valores do arquivo txt. No final libera memória da lista, encerra as threads e destroi o mutex.
void escalonadorPrioridade(){
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
    pthread_mutex_init(&mutex_prioridade, NULL);

    pthread_create(&lendo_novo_processo, NULL, &recebe_novos_processos, &numero_processos);
    pthread_create(&executando_processo, NULL, &executando_processos, &clock);

    pthread_join(executando_processo,NULL);

    pthread_cancel(lendo_novo_processo);

    pthread_mutex_destroy(&mutex_prioridade);

    criando_arquivo();

    free(lista_processosPR);
}

