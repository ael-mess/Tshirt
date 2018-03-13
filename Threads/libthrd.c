#include <pthread.h>
#include <stdio.h>

#include "libthrd.h"

int lanceThread(void (*thread)(void *), void *arg, int taille)
{
    pthreat_t tid;
    pthread_detach(tid); //compile with -pthread
    pthread_create(&tid, NULL, thread, arg);
    return tid;
}
