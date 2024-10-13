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
    int memoryAmount;
    int capacity;
    int *pageSequence;  
    int pageAmount;
    int latency;
    int positionCounter;
}process;

typedef struct st_memory{
    int processId;
    int page;
}st_memory;

process *cpu = NULL;
st_memory *memory = NULL;
pthread_mutex_t lockAC;

int numberP = 0; // número de processos
int counter = 0;
int sum_clocksAC = 0;
int memorySize = 0; // tamanho da memória
int memory_count = 0;
char method[50]; // política


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
    
    char line[100];
    char key[20];
    char name[50];
    int id, time, ticket;
    char intString[12];

    while (1){
        printf("\nCaso queira adicionar um novo processo digite nesse formato: nome|id|clock|prioridade \n\n");
        fgets(line, sizeof(line), stdin);
        pthread_mutex_lock(&lockAC);
        numberP++;
        counter++;
        cpu = realloc(cpu, numberP * sizeof(process));

        int result = sscanf(line, "%[^|]|%d|%d|%d", name, &id, &time, &ticket);
        if (result == 4) {
            strcpy(cpu[counter-1].name, name);
                cpu[counter-1].id = id;
                cpu[counter-1].clock = time;
                cpu[counter-1].ticket = ticket;
                cpu[counter-1].latency = 0;
                sum_clocksAC += cpu[counter-1].clock;
        printf("\nNovo processo adicionado: \n");
        printf("%s|%d|%d|%d \n", 
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
    int latency = 0;
    char line[100];

    const char *dirFile = "./SaidaAC.txt"; 
    FILE *out = fopen(dirFile, "w");

    if (out == NULL) {
        perror("Erro ao criar o arquivo");
    }

    while (1) {
        if(sum_clocksAC > 0 ){

            pthread_mutex_lock(&lockAC);
            for(int i = 0; i < numberP; i++) {
                if (cpu[i].clock > 0) {
                    printf("\nO processo %d está na CPU \n", cpu[i].id);

                    cpu[i].clock -= clock;
                    sum_clocksAC -= clock;
                    latency += clock;
                    cpu[i].latency = latency;
                    if (cpu[i].clock == 0){
                        printf("Processo %d finalizado\n", cpu[i].id);
                    }
                    printf("Resta %d de clock no processo %d \n", cpu[i].clock, cpu[i].id);
                    printf("O processo %d saiu da CPU \n", cpu[i].id);
                    sleep(1);
                }
            }
            pthread_mutex_unlock(&lockAC);
            sleep(1);
        } else {   
            if (cpu[numberP-1].id == -1) {
                printf("\nPrograma encerrado \n");
                break;
            }
            printf("\nCaso queira adicionar um novo processo digite nesse formato: nome|id|clock|Prioridade \n");
            printf("Caso queira encerrar digite (s) \n");
            sleep(3); 
        } 
    } 
           
    
    fprintf(out, "id|latência\n");
    
    for(int j = 0; j < counter; j++) {
        if(cpu[j].id != -1){
            fprintf(out, "%d|%d\n", cpu[j].id, cpu[j].latency);
        }
    }
    fclose(out);
    return NULL;
}

int handle(){
    return 8;
}

void read_numbers(process *p, char *str) {

    char *temp = strdup(str);
    char *token = strtok(temp, " ");
    p -> pageAmount = 0;
    
    while (token != NULL) {
        p -> pageAmount++;
        token = strtok(NULL, " ");
    }
    free(temp);

    p -> pageSequence = (int *)malloc(p -> pageAmount * sizeof(int));

    token = strtok(str, " ");
    int i = 0;
    while (token != NULL) {
        p -> pageSequence[i++] = atoi(token);
        token = strtok(NULL, " ");
    }
}

void remove_page(process *p) {
    if (p->pageAmount == 0) {
        printf("Nao ha paginas para remover.\n");
        return;
    }

    for (int i = 0; i < p->pageAmount - 1; i++) {
        p->pageSequence[i] = p->pageSequence[i + 1];
    }

    p->pageAmount--;

    p->pageSequence = (int *)realloc(p->pageSequence, p->pageAmount * sizeof(int));
}

int isPageInMemory(int page, int id) {
    
    for (int i = 0; i < memory_count; i++) {
        if (memory[i].page == page && memory[i].processId == id) {
            return 1;  
        }
    }
    return 0;
}

int findGreatReplacement(int pages[], int page_index, int total_pages) {
    int replace_index = -1; 
    int farthest = page_index;

    for (int i = 0; i < memory_count; i++) {
        int j;
        for (j = page_index; j < total_pages; j++) {
            if (memory[i].page == pages[j]) {
                if (j > farthest) {
                    farthest = j;  
                    replace_index = i;
                }
                break;
            }
        }
        if (j == total_pages) {
            return i;
        }
    }

    return (replace_index == -1) ? 0 : replace_index;
}

int insertPageInMemory(int process_id, int page, int page_id) {

    // if (strcmp(method, "local") == 0) {
    //     printf("Metodo local.\n");
    // } else {
    //     printf("Metodo global.\n");
    // } 

    if (!isPageInMemory(page, process_id)) {
        if (cpu[process_id].positionCounter <= cpu[process_id].capacity) {
            // if (memory_count < memorySize) {
                memory[memory_count].page = page;  
                memory[memory_count].processId = process_id; 
                memory_count++;
            // }  
            cpu[process_id].positionCounter++;
        } else {
            printf("pagina %d nao pode ser adicionada.\n", page);
        }
        // } else {
        //     int replace_index = findGreatReplacement(cpu[index].pageSequence, index + 1, cpu[index].pageAmount);
        //     memory[replace_index].page = page;
        // }
    }

    printf("Acesso a pagina %d -", page);
    for (int k = 0; k < memory_count; k++) {
        printf("%d - p%d  ", memory[k].page, memory[k].processId);
        
    }
    printf("\n");

}

void escalonadorAC() {
    pthread_t thread_exec, thread_add;
    const char *input = "entradaEscalonador1.txt";
    int clock, pageSize, allocation, access;
    // apagar depois
    int memory_Size;
    //
    char line[100];
    char name[50];
    int id, time, ticket, amount;
    char numbersSequence[500];

    pthread_mutex_init(&lockAC, NULL);
  
    FILE *file = fopen(input, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return;
    }

    numberP = handleNumberProcesses(input);
    fscanf(file, "%*[^|]|%d|%[^|]|%d|%d|%d|%d", &clock, method, &memory_Size, &pageSize, &allocation, &access);

    memorySize = 8;
    cpu = malloc(numberP * sizeof(process));
    memory = malloc(memorySize * sizeof(st_memory));

    if (cpu == NULL || memory == NULL) {
        printf("Erro na alocacao de cpu ou memoria.\n");
    }
    
    while (fgets(line, sizeof(line), file) != NULL) {
        int result = sscanf(line, "processo-%[^|]|%d|%d|%d|%d|%[^\n]", name, &id, &time, &ticket, &amount, numbersSequence);
        if (result == 6) { 
            
            strcpy(cpu[counter].name, name);
            cpu[counter].id = id;
            cpu[counter].clock = time;
            cpu[counter].ticket = ticket;
            cpu[counter].memoryAmount = amount;
            cpu[counter].capacity = ((amount / pageSize) * allocation / 100 );
            cpu[counter].latency = 0;
            cpu[counter].positionCounter = 1;
            sum_clocksAC += time;

            read_numbers(&cpu[counter], numbersSequence);

            counter++;
        }
        
    }

    // Excluir depois
    int all_zero = 0; 
    int latency = 0;
    // 

    while (!all_zero) {
        all_zero = 1; 

        for(int i = 0; i < counter; i++) {
            if (cpu[i].clock > 0) {
                printf("Processo - %d\n", cpu[i].id);
                if (cpu[i].clock < clock) {
                    
                    for (int j = 0; j < cpu[i].clock; j++) {
                        for (int k = 0; k < access; k++) {
                            insertPageInMemory(cpu[i].id, cpu[i].pageSequence[0], 1);
                            // printf("pg: %d\n", cpu[i].pageSequence[0]);
                            remove_page(&cpu[i]);
                        }
                    }
                    latency += cpu[i].clock;
                    cpu[i].latency = latency;
                    cpu[i].clock -= cpu[i].clock;  
                } else {
                    for (int j = 0; j < clock; j++) {
                        for (int k = 0; k < access; k++) {
                            insertPageInMemory(cpu[i].id, cpu[i].pageSequence[0], 1);
                            // printf("pg: %d\n", cpu[i].pageSequence[0]);
                            remove_page(&cpu[i]);
                        }
                    }
                    cpu[i].clock -= clock;  
                    latency += clock;
                    cpu[i].latency = latency;
                }
                // printf("id:%d|%d\n", cpu[i].id, cpu[i].clock);
                all_zero = 0;
            }
        }
        printf("\n");
    }


    // for (int i = 0; i < counter; i++) {
    //     printf("Processo: %s\n", cpu[i].name);
    //     printf("ID: %d\n", cpu[i].id);
    //     printf("Clock: %d\n", cpu[i].clock);
    //     printf("Ticket: %d\n", cpu[i].ticket);
    //     printf("Memoria: %d\n", cpu[i].memoryAmount);
    //      printf("Capacidade: %d\n", cpu[i].capacity);
        
    //     printf("Numeros: ");
    //     for (int j = 0; j < cpu[i].pageAmount; j++) {
    //         printf("%d ", cpu[i].pageSequence[j]);
    //     }
    //     printf("\n");

    // }


    // pthread_create(&thread_add, NULL, &handleAddProcess, NULL);
    // pthread_create(&thread_exec, NULL, &handleExecute, &clock);

    // pthread_join(thread_exec, NULL);

    // pthread_cancel(thread_add);

    free(cpu);
    pthread_mutex_destroy(&lockAC);
    fclose(file);
}

// printf("Processo-%s|%d|%d|%d\n", 
            //    cpu[counter].name, 
            //    cpu[counter].id, 
            //    cpu[counter].clock, 
            //    cpu[counter].ticket);