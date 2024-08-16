#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

typedef struct st_process{
    char name[50];
    int id;
    int clock;
    int ticket;
    int latency;
}process;

process *cpu = NULL;
pthread_mutex_t lockAC;

int numberP = 0; // número de processos
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

void handleRemoveElement(int list[], int size, int index) {
    if (index < 0 || index >= size) {
        printf("Índice fora dos limites.\n");
        return;
    }

    for (int i = index; i < size - 1; i++) {
        list[i] = list[i + 1];
    }
    
    size--;
    list[size] = 0;
}

void *handleAddProcess(void* arg){

    printf("\n");
    

    char line[100];
    char key[20];
    char name[50];
    int id, time, ticket;
    char intString[12];

    while (1){
        printf("Caso queira adicionar um novo processo digite nesse formato: nome|id|clock|bilhetes \n\n");
        fgets(line, sizeof(line), stdin);
        pthread_mutex_lock(&lockAC);
        numberP++;
        counter++;
        cpu = realloc(cpu, numberP * sizeof(process));

        int result = sscanf(line, "processo-%[^|]|%d|%d|%d", name, &id, &time, &ticket);
        if (result == 4) {
        strcpy(cpu[counter-1].name, name);
            cpu[counter-1].id = id;
            cpu[counter-1].clock = time;
            cpu[counter-1].ticket = ticket;
            cpu[counter-1].latency = 0;
        printf("\n");
        printf("Novo processo adicionado: \n");
        printf("Processo-%s|%d|%d|%d", 
            cpu[counter-1].name, 
            cpu[counter-1].id, 
            cpu[counter-1].clock, 
            cpu[counter-1].ticket);
        } else {
            int result = sscanf(line, "%s", key);

            if(result == 1 && (strcmp(key, "s") == 0 || strcmp(key, "S") == 0)){
                cpu[counter-1].id = -1;
                break;
            }
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
    char line[100];

    const char *dirFile = "./SaidaAC.txt"; 
    FILE *out = fopen(dirFile, "w");

    if (out == NULL) {
        perror("Erro ao criar o arquivo");
    }

    sleep(2);
    while (!all_zero) {
        
        /* all_zero = 1;  */
        pthread_mutex_lock(&lockAC);
        for(int i = 0; i < counter; i++) {
            if (cpu[i].clock > 0) {
                printf("O processo %d está na CPU \n", cpu[i].id);
                if (cpu[i].clock < clock) {
                    latency += cpu[i].clock;
                    cpu[i].latency = latency;
                    cpu[i].clock -= cpu[i].clock;  
                } else {
                    cpu[i].clock -= clock;  
                    latency += clock;
                    cpu[i].latency = latency;
                }
                /* all_zero = 0;  */
                printf("id:%d|%d\n", cpu[i].id, cpu[i].clock);
                printf("O processo %d saiu da CPU \n", cpu[i].id);
            } /* else {
                
                if (cpu[i].id == -1) {
                    break;
                }
                
            } */
            printf("Processo %d finalizado", cpu[i].id);
            printf("\n");
            sleep(1); 
            printf("\nCaso queira adicionar um novo processo digite nesse formato: nome|id|clock|Prioridade \n");
            printf("Caso queira encerrar digite (s) \n");
            sleep(2);
            printf("\n");
        } 
           
        printf("\n");
        pthread_mutex_unlock(&lockAC);
    }
    
    fprintf(out, "id|latência\n");
    
    for(int j = 0; j < counter; j++) {
        fprintf(out, "%d|%d\n", cpu[j].id, cpu[j].latency);
    }
    fclose(out);
    return NULL;
}


void escalonadorAC() {
    pthread_t thread_exec, thread_add;
    const char *input = "entradaEscalonador1.txt";
    int clock;
    char line[100];
    char name[50];
    int id, time, ticket;

    pthread_mutex_init(&lockAC, NULL);

    
    FILE *file = fopen(input, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    numberP = handleNumberProcesses(input);
    fscanf(file, "%*[^|]|%d", &clock);

    cpu = malloc(numberP * sizeof(process));
    
    while (fgets(line, sizeof(line), file) != NULL) {
        int result = sscanf(line, "processo-%[^|]|%d|%d|%d", name, &id, &time, &ticket);
        if (result == 4) { 
            
            strcpy(cpu[counter].name, name);
            cpu[counter].id = id;
            cpu[counter].clock = time;
            cpu[counter].ticket = ticket;
            cpu[counter].latency = 0;
            printf("Processo-%s|%d|%d|%d\n", 
               cpu[counter].name, 
               cpu[counter].id, 
               cpu[counter].clock, 
               cpu[counter].ticket);
            counter++;
        }
        sleep(1);
    } 

    pthread_create(&thread_add, NULL, &handleAddProcess, NULL);
    pthread_create(&thread_exec, NULL, &handleExecute, &clock);

    pthread_join(thread_exec, NULL);

    pthread_cancel(thread_add);

    free(cpu);
    pthread_mutex_destroy(&lockAC);
    fclose(file);
}