#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Declarando a estrutura de um processo
typedef struct st_process{
    char name[50];
    int id;
    int clock;
    int ticket;
    int latency;
}process;

int handleProcesses(const char *entrada){
    FILE *file = fopen(entrada, "r");
    int contador = 0;
    char c, letra = '\n';
    while(fread (&c, sizeof(char), 1, file)) {
        if(c == letra) {
            contador++;
        }
    }
    fclose(file);
    return contador-1;
}


int main() {

    // Declarando variáveis
    const char *entrada = "entradaEscalonador1.txt";
    int numberP = handleProcesses(entrada);
    int clock;
    char line[100];
    char name[100];
    int id, time, ticket;
    int counter = 0;
    int all_zero = 0; 

    process *list = (process *)malloc(numberP * sizeof(process));
    
    FILE *file = fopen(entrada, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    fscanf(file, "%*[^|]|%d", &clock);
    
    while (fgets(line, sizeof(line), file) != NULL) {

        int result = sscanf(line, "processo-%[^|]|%d|%d|%d", name, &id, &time, &ticket);
        if (result == 4) { 
            strcpy(list[counter].name, name);
            list[counter].id = id;
            list[counter].clock = time;
            list[counter].ticket = ticket;
            list[counter].latency = 0;
            printf("Processo %s|%d|%d|%d\n", 
               list[counter].name, 
               list[counter].id, 
               list[counter].clock, 
               list[counter].ticket);
        }
        printf("\n");
        counter++;
    } 
    
    while (!all_zero) {
        all_zero = 1; 
        int latency = 0;

        for(int i = 1; i < counter; i++) {
            if (list[i].clock > 0) {
                if (list[i].clock < clock) {
                    latency += list[i].clock;
                    list[i].latency += latency;
                    list[i].clock -= list[i].clock;  
                } else {
                    list[i].clock -= clock;  
                    latency += clock;
                    list[i].latency += latency;
                }
                /* printf("id:%d|%d| lat: %d \n", list[i].id, list[i].clock, list[i].latency); */
                all_zero = 0;
            }
            printf("id:%d|%d| lat: %d \n", list[i].id, list[i].clock, list[i].latency);
        }
        
        printf("\n");
    }

    free(list);
    fclose(file);
    return 0;
}