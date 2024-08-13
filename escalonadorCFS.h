#ifndef ESCALONADORCFS_H
#define ESCALONADORCFS_H

#include <pthread.h>

typedef struct {
    char name[20];
    int id,
        clock,
        priority,
        isAvailable;
} Process;

extern pthread_mutex_t mutex;


void escalonadorCFS(Process process);  // Declaração de funcao1

#endif