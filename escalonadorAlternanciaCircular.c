#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct st_process{
    char name[50];
    int id;
    int clock;
    int ticket;
    int latency;
}process;

process *list = NULL;
pthread_mutex_t lockAC;

int numberP = 0;
int counter = 0;

int handleNumberProcesses(const char *input){
    int counter = 0;
    char c, letter = '\n';
    FILE *file = fopen(input, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }
    while(fread (&c, sizeof(char), 1, file)) {
        if(c == letter) {
            counter++;
        }
    }
    fclose(file);
    counter -= 1;
    return counter;
}

void *handleAddProcess(void* arg){

    printf("\n");
    printf("Caso queira adicionar um novo processo digite nesse formato: nome|id|clock|bilhetes \n\n");

    char line[100];
    char name[100];
    int id, time, ticket;
    char intString[12];

    while (1){
        pthread_mutex_lock(&lockAC);
        numberP++;
        list = realloc(list, numberP * sizeof(process));

        fgets(line, sizeof(line), stdin);

        int result = sscanf(line, "processo-%[^|]|%d|%d|%d", name, &id, &time, &ticket);
        if (result == 4) {
        strcpy(list[numberP].name, name);
            list[numberP].id = id;
            list[numberP].clock = time;
            list[numberP].ticket = ticket;
            list[numberP].latency = 0;
        printf("\n");
        printf("Novo processo adicionado: \n");
        printf("Processo-%s|%d|%d|%d", 
            list[numberP].name, 
            list[numberP].id, 
            list[numberP].clock, 
            list[numberP].ticket);
        }
        pthread_mutex_unlock(&lockAC);
    }
    
	return NULL;
}

void* handleExecute(void* arg) {
    int *cpu_clock_ptr = (int *)arg;
    int clock = *cpu_clock_ptr;
    int all_zero = 0; 
    int latency = 0;

    while (!all_zero) {
        pthread_mutex_lock(&lockAC);
        all_zero = 1; 

        for(int i = 1; i < counter; i++) {
            if (list[i].clock > 0) {
                printf("O processo %d está na CPU \n", list[i].id);
                if (list[i].clock < clock) {
                    latency += list[i].clock;
                    list[i].latency = latency;
                    list[i].clock -= list[i].clock;  
                } else {
                    list[i].clock -= clock;  
                    latency += clock;
                    list[i].latency = latency;
                }
                printf("id:%d|%d\n", list[i].id, list[i].clock);
                all_zero = 0;
            }
            printf("Processo %d finalizado \n", list[i].id);
        }    
        printf("\n");
        pthread_mutex_unlock(&lockAC);
    }
    const char *dirFile = "./resultadoAlternanciaCircular.txt"; 
    FILE *out = fopen(dirFile, "w");

    if (out == NULL) {
        perror("Erro ao criar o arquivo");
    }
    fprintf(out, "id|latência\n");
    
    for(int j = 1; j < counter; j++) {
        fprintf(out, "%d|%d\n", list[j].id, list[j].latency);
    }
    fclose(out);
    return NULL;
}


void escalonadorAC() {
    pthread_t thread_exec, thread_add;
    const char *input = "entradaEscalonador1.txt";
    int clock;
    char line[100];
    char name[100];
    int id, time, ticket;

    pthread_mutex_init(&lockAC, NULL);

    list = (process *)malloc(numberP * sizeof(process));
    
    FILE *file = fopen(input, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    numberP = handleNumberProcesses(input);
    fscanf(file, "%*[^|]|%d", &clock);
    
    while (fgets(line, sizeof(line), file) != NULL) {

        int result = sscanf(line, "processo-%[^|]|%d|%d|%d", name, &id, &time, &ticket);
        if (result == 4) { 
            strcpy(list[counter].name, name);
            list[counter].id = id;
            list[counter].clock = time;
            list[counter].ticket = ticket;
            list[counter].latency = 0;
            printf("Processo-%s|%d|%d|%d\n", 
               list[counter].name, 
               list[counter].id, 
               list[counter].clock, 
               list[counter].ticket);
            counter++;
        }
        printf("\n");
        
    } 

    pthread_create(&thread_add, NULL, &handleAddProcess, NULL);
    pthread_create(&thread_exec, NULL, &handleExecute, &clock);

    pthread_join(thread_exec, NULL);

    pthread_cancel(thread_add);

    free(list);
    pthread_mutex_destroy(&lockAC);
    fclose(file);
}