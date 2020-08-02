#include "thread.h"
#include <pthread.h>
#include <stdlib.h>

void *entryPoint(void *pthis) {
    struct thread *thread = (struct thread *)pthis;
    thread->run(thread->args);
    free(thread);
}

struct thread *thread_start(void (*run)(void *), void *args) {
    struct thread *thread = (struct thread *)malloc(sizeof(struct thread));
    thread->run = run;
    thread->args = args;
    if (pthread_create(&thread->m_threadId, NULL, entryPoint, thread)) {
        free(thread);
        return NULL;
    }
    return thread;
}

int thread_wait(struct thread *thread) {
    return pthread_join(thread->m_threadId, NULL);
}

int thread_abort(struct thread *thread) {
    int ret = pthread_cancel(thread->m_threadId);
    if (ret) return ret;
    free(thread);
    return 0;
}

int thread_detach(struct thread *thread) {
    return pthread_detach(thread->m_threadId);
}

pthread_t thread_getid(struct thread *thread) {
    return thread->m_threadId;
}