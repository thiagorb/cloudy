#ifndef THREAD_H
#define	THREAD_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <pthread.h>

struct thread {
    pthread_t m_threadId;
    void (*run)();
    void* args;
};

struct thread *thread_start(void (*run)(void *), void *args);
int thread_wait(struct thread *thread);
int thread_abort(struct thread *thread);
int thread_detach(struct thread *thread);
pthread_t thread_getid(struct thread *thread);

#ifdef	__cplusplus
}
#endif

#endif	/* THREAD_H */

