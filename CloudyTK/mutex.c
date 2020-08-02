#include <stdlib.h>
#include <pthread.h>
#include "mutex.h"

struct mutex *mutex_create() {
    struct mutex *mutex = (struct mutex *)malloc(sizeof(struct mutex));
    pthread_mutex_init(&mutex->mutex, NULL);
    return mutex;
}

void mutex_lock(struct mutex *mutex) {
    pthread_mutex_lock(&mutex->mutex);
}

void mutex_unlock(struct mutex *mutex) {
    pthread_mutex_unlock(&mutex->mutex);
}

void mutex_destroy(struct mutex *mutex) {
    pthread_mutex_destroy(&mutex->mutex);
    free(mutex);
}