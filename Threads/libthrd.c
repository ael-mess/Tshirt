#include <pthread.h>
#include <stdio.h>

#include "libthrd.h"

int lanceThread(void *(*thread)(void *), void *arg)
{
    pthread_t tid;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&tid, &attr, thread, arg);

    /*int chk;
    pthread_attr_getdetachstate(&attr, &chk);
    if(chk == PTHREAD_CREATE_DETACHED) printf("    Detached\n");*/
    return 0;
}
