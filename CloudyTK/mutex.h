#ifndef MUTEX_H
#define	MUTEX_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#include <pthread.h>

struct mutex {
    pthread_mutex_t mutex;
};

struct mutex *mutex_create();
void mutex_lock(struct mutex *mutex);
void mutex_unlock(struct mutex *mutex);
void mutex_destroy(struct mutex *mutex);

#ifdef	__cplusplus
}
#endif

#endif	/* MUTEX_H */

