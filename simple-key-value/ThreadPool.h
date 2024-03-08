#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#define MAX_THREADS 64
#define MAX_QUEUE 65536

enum threadpool_error_t
{
    threadpool_invalid = -1,
    threadpool_lock_failure = -2,
    threadpool_queue_full = -3,
    threadpool_shutdown = -4,
    threadpool_thread_failure = -5
};

enum threadpool_shutdown_t
{
    immediate_shutdown = 1,
    graceful_shutdown = 2
};

// task of taskqueue
struct threadpool_task_t
{
    void (*function)(void *);
    void *argument;
};

struct threadpool_t
{
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t *threads;
    threadpool_task_t *queue;
    int thread_count;
    int queue_size;
    int head;
    int tail;
    int count;
    int shutdown;
    int started;
};

enum threadpool_destroy_flags_t
{
    threadpool_graceful = 1
};

threadpool_t *threadpool_create(int thread_count, int queue_size, int flags);

int threadpool_add(threadpool_t *pool, void (*routine)(void *), void *arg, int flags);

int threadpool_destroy(threadpool_t *pool, int flags);

#endif /* _THREADPOOL_H_ */