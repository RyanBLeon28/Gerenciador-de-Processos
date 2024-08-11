#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <glib.h>       // Biblioteca para Red-Black Tree

typedef struct {
    int clock_cpu;
} ClockCPU;

typedef struct {
    char name[20];
    int id,
        clock,
        priority;
} Process;

typedef struct {
    GTree *tree;
    ClockCPU *clock;
    GList *processes_to_remove;
} ExecData;

// Função de comparação usada pelo GTree
gint compare_ints(gconstpointer a, gconstpointer b) {
    return (*(const int *)a - *(const int *)b);
}

// Função para imprimir chave/valor
gboolean print_key_value(gpointer key, gpointer value, gpointer user_data) {
    Process *process = value;
    printf("Prioridade: %d, ID: %d, Nome: %s, Clock: %d, Prioridade: %d\n",
           *(int *)key, process->id, process->name, process->clock, process->priority);
    return FALSE;
}

// Funcao para remover os processos
void removeProcess(ExecData *data) {
    GList *iter = data->processes_to_remove;
    while (iter != NULL) {
        Process *process = (Process *)iter->data;
        gpointer key = &(process->priority);
        g_tree_remove(data->tree, key);
        iter = iter->next;
        printf("Processo ID: %d encerrou\n", process->id);
    }
    g_list_free(data->processes_to_remove);
    data->processes_to_remove = NULL;
}

// Funcao para executar o processo dentro da CPU e remover o tempo utilizado
gboolean execProcess(gpointer key, gpointer value, gpointer user_data) {
    ExecData *data = (ExecData *)user_data;
    GTree *tree = data->tree;
    ClockCPU *clock = data->clock;

    Process *process = value;
    printf("\n Processo %d entrou na CPU \n", process->id);
    
    int restTime = process->clock - clock->clock_cpu;
    //printf("Tempo de CPU %d\n", clock->clock_cpu);
    printf("Tempo anterior: %d, Tempo restante: %d\n", process->clock, restTime);
    process->clock = restTime;

    //Fim do processo, então é removido da árvore
    if ( process->clock <= 0){
        data->processes_to_remove = g_list_prepend(data->processes_to_remove, process);
    }
    sleep(1);
    return FALSE;
}

int main(){
    srand(time(NULL));
    FILE *fp = fopen("entradaEscalonador1.txt", "r");
    if (fp == NULL) {
        perror("Erro ao abrir o arquivo");
        exit(EXIT_FAILURE);
    }
    char ch, *processos, *alg;
    int count=0, clock_cpu;

    char linha[100];

    //Mostra qual o algoritmo e o clock
    while (fgets(linha, sizeof(linha), fp)) { 
        alg = strtok(linha, "|");
        printf("O algoritmo é: %s\n", alg);
        clock_cpu = atoi(strtok(NULL, "|"));
        break;
    }

    printf("A fração da CPU será: %d\n", clock_cpu);
    ClockCPU *clock = malloc(sizeof(ClockCPU));
    clock->clock_cpu = clock_cpu;

    // Cria a árvore Red-Black
    GTree *tree = g_tree_new(compare_ints);

    while (fgets(linha, sizeof(linha), fp)){
        Process *process = malloc(sizeof(Process));
        strcpy(process->name,strtok(linha, "|"));
        process->id = atoi(strtok(NULL, "|"));
        process->clock = atoi(strtok(NULL, "|"));
        process->priority = atoi(strtok(NULL, "\n"));

        int *priority_copy = malloc(sizeof(int));
        *priority_copy = process->priority;

        g_tree_insert(tree, priority_copy, process);
        count++;
    };
    // Percorrendo e imprimindo a árvore apos toda a insercao
    printf("\n Lista de processos\n");
    g_tree_foreach(tree, print_key_value, NULL);

    printf("\n----------Inicio do escalonamento CFS----------\n");

    while (TRUE){ 
        ExecData data = {tree, clock, NULL};
        g_tree_foreach(tree, execProcess, &data); // Processo entra na CPU 
        removeProcess(&data);

        g_tree_foreach(tree, print_key_value, NULL); //Printa a arvore a cada ciclo 

        if (g_tree_nnodes(tree) == 0) {
            printf("CPU vazia.\n");
        }
        sleep(2);
    } 

    // Liberando a árvore, a estrutura e o arquivo
    g_tree_destroy(tree);
    free(clock);
    fclose(fp);

    return 0;
}
