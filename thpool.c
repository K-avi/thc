#include "thpool.h"
#include "common.h"
#include "job_deque.h"
#include <pthread.h>
#include <stdint.h>

#define DEF_THREAD_COUNT 4

#define pool_running 1<<0
#define pool_shutting_down_nice 1<<1
#define pool_shutting_down_force 1<<2

static void * thread_pool_thread(void * pool){
    //def_err_handler(!pool, "thread_pool_thread", err_null)
    S_THPOOL * thpool = (S_THPOOL *)pool;
    S_TASK fn_task;
    while(1){
        pthread_mutex_lock(&thpool->mutex_queue);
        //def_err_handler(failure, "thread_pool_thread", failure);

        while(thpool->task_queue.size == 0){
            pthread_cond_wait(&thpool->cond_queue, &thpool->mutex_queue);
            //def_err_handler(failure, "thread_pool_thread", failure);
        }
        task_queue_pop(&thpool->task_queue, &fn_task);
        //def_err_handler( (void*)failure, "thread_pool_thread", failure);

        pthread_mutex_unlock(&thpool->mutex_queue);
        //def_err_handler( (void*)failure, "thread_pool_thread", failure);

        fn_task.function(fn_task.args);
    }
    return (void*) err_ok;
}//not tested



err_code thtab_init(S_THTAB * thtab){
    def_err_handler(!thtab, "thtab_init", err_null)

    thtab->threads = (pthread_t *)calloc(DEF_THREAD_COUNT, sizeof(pthread_t));
    def_err_handler(!thtab->threads, "thtab_init", err_alloc);

    thtab->size = DEF_THREAD_COUNT;

    return err_ok;
}//not tested

err_code thpool_create(S_THPOOL * pool ){
    def_err_handler(!pool, "thpool_create", err_null)
    //initialize the threads
    err_code failure = thtab_init(&pool->thtab);
    def_err_handler(failure, "thpool_create", failure);
    //initialize the deque
    failure = task_queue_init(&pool->task_queue);
    def_err_handler(failure, "thpool_create", failure);
    
    //initialize the mutex and the condition
    failure = pthread_mutex_init(&pool->mutex_queue, NULL) ;
    def_err_handler(failure, "thpool_create", err_null);
    failure = pthread_cond_init(&pool->cond_queue, NULL);
    def_err_handler(failure, "thpool_create", err_null);

    for(uint32_t i = 0 ; i < pool->thtab.size ; i++){
        failure = pthread_create(&pool->thtab.threads[i], NULL, thread_pool_thread, pool);
        def_err_handler(failure, "thpool_create", failure);
    }

    pool->flags = 1<<0 ; 

    return err_ok;
}//not tested

err_code thpool_append_task(S_THPOOL * pool , S_TASK * task){
    def_err_handler(! (pool && task), "thpool_append_task", err_null)

    //acquire the lock
    err_code failure =  pthread_mutex_lock(&pool->mutex_queue);
    def_err_handler(failure, "thpool_append_task", failure);
   
    if(!(pool->flags & 
        (pool_shutting_down_force | 
        pool_shutting_down_nice ))){ 
        //if the pool isn't shutting down

        //push to the back of the queue
        failure = task_queue_push(&pool->task_queue, task);
        def_err_handler(failure, "thpool_append_task", failure);
    }
    
    //unlock and signal worker threads
    pthread_mutex_unlock(&pool->mutex_queue);
    pthread_cond_signal(&pool->cond_queue);

    return err_ok ;
}//not tested


void thpool_destroy(S_THPOOL * pool, uint16_t mask ){

}

