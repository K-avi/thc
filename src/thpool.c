#include "thpool.h"
#include "common.h"
#include "job_deque.h"

#include <pthread.h>
#include <stdint.h>

#define DEF_THREAD_COUNT 4

#define pool_running 1<<0
#define pool_shutting_down_nice 1<<1
#define pool_shutting_down_force 1<<2

#define pool_shutting_down 0x6

static void * thread_pool_thread(void * pool){
    //def_err_handler(!pool, "thread_pool_thread", err_null)
    S_THPOOL * thpool = (S_THPOOL *)pool;
    S_TASK fn_task;
    while(! (thpool->flags & pool_shutting_down) ){
        pthread_mutex_lock(&thpool->mutex_queue);

        while( task_queue_empty(&thpool->task_queue)  && 
                !(thpool->flags & pool_shutting_down)){
            pthread_cond_wait(&thpool->cond_queue, &thpool->mutex_queue);
        }

        pthread_mutex_lock(&thpool->mutex_nbworking); 
        thpool->nb_th_working++;
        pthread_mutex_unlock(&thpool->mutex_nbworking);

        if(thpool->flags & pool_shutting_down_force){
            pthread_mutex_unlock(&thpool->mutex_queue);
            pthread_exit(NULL);
        }

        task_queue_pop(&thpool->task_queue, &fn_task);
        pthread_mutex_unlock(&thpool->mutex_queue);
        if(fn_task.args && fn_task.function ){
            fn_task.function(fn_task.args);
        }
        
        pthread_mutex_lock(&thpool->mutex_nbworking); 
        thpool->nb_th_working--;
        if(!thpool->nb_th_working){
            pthread_cond_signal(&thpool->cond_nbworking);
        }
        pthread_mutex_unlock(&thpool->mutex_nbworking);
    }
    
    pthread_exit(NULL); 

    return (void*) err_ok;
}//tested ; messy ; seems ok 

static err_code thtab_init(S_THTAB * thtab){
    def_err_handler(!thtab, "thtab_init", err_null)

    thtab->threads = (pthread_t *)calloc(DEF_THREAD_COUNT, sizeof(pthread_t));
    def_err_handler(!thtab->threads, "thtab_init", err_alloc);

    thtab->size = DEF_THREAD_COUNT;

    return err_ok;
}//tested ; ok 

static void thtab_free(S_THTAB * thtab){
   if(thtab)
    if (thtab->threads)
    free(thtab->threads);
}//tested ; ok

err_code thpool_init(S_THPOOL * pool ){
    def_err_handler(!pool, "thpool_create", err_null)
    //initialize the threads
    err_code failure = thtab_init(&pool->thtab);
    def_err_handler(failure, "thpool_create", failure);
    //initialize the deque
    failure = task_queue_init(&pool->task_queue);
    def_err_handler(failure, "thpool_create", failure);
    
    //initialize the mutex and the condition of the queue 
    failure = pthread_mutex_init(&pool->mutex_queue, NULL) ;
    def_err_handler(failure, "thpool_create", err_null);
    failure = pthread_cond_init(&pool->cond_queue, NULL);
    def_err_handler(failure, "thpool_create", err_null);

    //initialize the mutex / conition of the nb threads working 
    failure = pthread_mutex_init(&pool->mutex_nbworking, NULL);
    def_err_handler(failure, "thpool_init", failure);
    failure = pthread_cond_init(&pool->cond_nbworking, NULL);
    def_err_handler(failure, "thpool_create", err_null);

    pool->nb_th_working = 0 ; 
    pool->flags = 1<<0 ; 

    for(uint32_t i = 0 ; i < pool->thtab.size ; i++){
        failure = pthread_create(&pool->thtab.threads[i], NULL, thread_pool_thread, pool);
        def_err_handler(failure, "thpool_create", failure);
    }

    return err_ok;
}//tested ; ok 

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
}//tested ; ok


static err_code thpool_wait_for_all(S_THPOOL * pool){

    pthread_mutex_lock(&pool->mutex_nbworking);
    while(pool->nb_th_working)
        pthread_cond_wait(&pool->cond_nbworking, &pool->mutex_nbworking);
    pthread_mutex_unlock(&pool->mutex_nbworking);

    return err_ok; 
}//tested ; ok

void thpool_destroy(S_THPOOL * pool, uint16_t mask ){

    //is it really critical ? idk
    //pthread_mutex_lock(&pool->mutex_nbworking);
    pool->flags |= mask;
    //pthread_mutex_unlock(&pool->mutex_nbworking);

    if(mask & pool_shutting_down_force){
        pthread_cond_broadcast(&pool->cond_queue);
        for(uint32_t i = 0 ; i < pool->thtab.size ; i++){
            pthread_join(pool->thtab.threads[i], NULL);
        }
    }else if( mask & pool_shutting_down_nice){
        thpool_wait_for_all(pool); 
        pthread_cond_broadcast(&pool->cond_queue);
        for(uint32_t i = 0 ; i < pool->thtab.size ; i++){
            pthread_join(pool->thtab.threads[i], NULL);
        }
    }

    pthread_mutex_destroy(&pool->mutex_queue);
    pthread_cond_destroy(&pool->cond_queue);
    pthread_mutex_destroy(&pool->mutex_nbworking);
    pthread_cond_destroy(&pool->cond_nbworking);
    thtab_free(&pool->thtab);
    task_queue_destroy(&pool->task_queue);
}//tested ; ok 
//there should be another way to shutdown 
//the pool where you wait for all pending jobs 
//to be completed 


