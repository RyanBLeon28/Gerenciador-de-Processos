#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct st_process{
    char name[50];
    int id;
    int clock;
    int ticket;
    int latency;
}process;

int handleProcesses(const char *input){
    FILE *file = fopen(input, "r");
    int counter = 0;
    char c, letter = '\n';
    while(fread (&c, sizeof(char), 1, file)) {
        if(c == letter) {
            counter++;
        }
    }
    fclose(file);
    return counter-1;
}


int main() {

    const char *input = "entradaEscalonador1.txt";
    int numberP = handleProcesses(input);
    int clock;
    char line[100];
    char name[100];
    int id, time, ticket;
    int latency = 0;
    int counter = 0;
    int all_zero = 0; 

    process *list = (process *)malloc(numberP * sizeof(process));
    
    FILE *file = fopen(input, "r");
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

        for(int i = 1; i < counter; i++) {
            if (list[i].clock > 0) {
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
        }
        
        printf("\n");
    }
    const char *dirFile = "./resultadoAlternanciaCircular.txt"; 
    FILE *out = fopen(dirFile, "w");

    if (out == NULL) {
        perror("Erro ao criar o arquivo");
        return 1;
    }
    fprintf(out, "id|latência\n");
    
    for(int j = 1; j < counter; j++) {
        fprintf(out, "%d|%d\n", list[j].id, list[j].latency);
    }
    fclose(out);

    free(list);
    fclose(file);
    return 0;
}