#include "thpool.h"
#include "common.h"
#include "job_deque.h"

#define DEF_THREAD_COUNT 4
uint32_t global_nb_threads = DEF_THREAD_COUNT;

#define f_running 1
#define f_accepting 2
#define f_shutdown 4

/* STATIC THTAB MANIPULATION */
static err_code init_thtab(S_THTAB * thtab){
    def_err_handler(!thtab, "thtab_init", err_null)

    thtab->threads = (pthread_t *)calloc(global_nb_threads, sizeof(pthread_t));
    def_err_handler(!thtab->threads, "thtab_init", err_alloc)

    thtab->size =global_nb_threads;
    return err_ok;
}//yeah

static void free_thtab(S_THTAB * thtab){
    if(thtab){
        if(thtab->threads){
            free(thtab->threads); 
        }
        thtab->threads = NULL ; 
        thtab->size = 0 ; 
    }
}//yeah

/* THPOOL PRIMITIVES */
err_code thpool_init(S_THPOOL * pool ){
    def_err_handler(!pool, "thpool_init", err_null);

    err_code failure = init_thtab(&pool->thtab);
    def_err_handler(failure, "thpool_init", failure);

    failure = task_queue_init(&pool->task_queue);
    def_err_handler(failure, "thpool_init", failure);

    pool->flags = f_running | f_accepting;
    pool->nb_th_working = 0;

    pool->nb_submitted = pool->nb_completed = 0;

    pthread_mutex_init(&pool->mutex_queue, NULL);
    pthread_cond_init(&pool->cond_queue, NULL);

    pthread_mutex_init(&pool->mutex_sub_equals_comp, NULL);
    pthread_cond_init(&pool->cond_sub_equals_comp, NULL);

    return err_ok ;
}


static void * thpool_worker(void * arg) ; 
err_code thpool_start(S_THPOOL *pool){
    def_err_handler(!pool, "thpool_start", err_null);

    for(uint32_t i = 0 ; i < pool->thtab.size ; i++){
        int err = pthread_create(&pool->thtab.threads[i],
             NULL, thpool_worker, pool);
        def_err_handler(err, "thpool_start", err_thstart);
    }
    return err_ok;
}

err_code thpool_append_task(S_THPOOL *pool, S_TASK *task){
    def_err_handler(!pool, "thpool_append_task", err_null);
    def_err_handler(!task, "thpool_append_task", err_null);
    
    if((! (pool->flags & f_accepting) )){
        return err_ok;
    }
    pthread_mutex_lock(&pool->mutex_queue);
    pool->nb_submitted ++ ; 
    err_code failure = task_queue_push(&pool->task_queue, task);
    def_err_handler(failure, "thpool_append_task", failure);
    
    pthread_mutex_unlock(&pool->mutex_queue);
    pthread_cond_signal(&pool->cond_queue);
    
    return err_ok;
}


err_code thpool_append_tasks(S_THPOOL *pool, S_TASK *tasks, uint32_t nb_tasks){
    def_err_handler(!pool, "thpool_append_tasks", err_null);
    def_err_handler(!tasks, "thpool_append_tasks", err_null);
    if((! (pool->flags & f_accepting) )){
        return err_ok;
    }
    
    for(uint32_t i = 0 ; i < nb_tasks ; i++){
        thpool_append_task(pool, &tasks[i]);
    }


    return err_ok;
}


err_code thpool_wait_for_all(S_THPOOL * pool){
    def_err_handler(!pool, "thpool_wait_for_all", err_null);
    pool->flags |= !f_accepting;
    //printf("wait\n ");
    pthread_mutex_lock(&pool->mutex_sub_equals_comp);
    while( pool->nb_completed < pool->nb_submitted){
        pthread_cond_wait(&pool->cond_sub_equals_comp, &pool->mutex_sub_equals_comp);
    }
    pthread_mutex_unlock(&pool->mutex_sub_equals_comp);
    //printf("out of wait %d %d\n", pool->nb_submitted , pool->nb_completed);
    

    return err_ok ;
}

err_code thpool_restart(S_THPOOL * pool){
    def_err_handler(!pool, "thpool_restart", err_null); 

    pthread_mutex_lock(&pool->mutex_sub_equals_comp);
    while( pool->nb_completed < pool->nb_submitted){
        pthread_cond_wait(&pool->cond_sub_equals_comp, &pool->mutex_sub_equals_comp);
    }
    pthread_mutex_unlock(&pool->mutex_sub_equals_comp);
    pool->flags |= f_accepting;
    //pool->nb_submitted = pool->nb_completed = 0 ;

    return err_ok;
}

err_code thpool_destroy(S_THPOOL *pool){
    def_err_handler(!pool, "thpool_destroy", err_null);

    thpool_wait_for_all(pool);  
    pool->flags |= f_shutdown;

    pthread_cond_broadcast(&pool->cond_queue);
    for(uint32_t i = 0 ; i < pool->thtab.size ; i++){
        pthread_join(pool->thtab.threads[i], NULL);
    }

    free_thtab(&pool->thtab);
    task_queue_destroy(&pool->task_queue);
    pthread_mutex_destroy(&pool->mutex_queue);
    pthread_cond_destroy(&pool->cond_queue);

    return err_ok;
}

/* THE ACTUAL WORKER FUNCTION */

static void * thpool_worker(void * arg){
    S_THPOOL * pool = (S_THPOOL *) arg;
    S_TASK task;

    while(1){
        
        pthread_mutex_lock(&pool->mutex_queue);
        while(! pool->task_queue.size && !(pool->flags & f_shutdown) ){
            pthread_cond_wait(&pool->cond_queue, &pool->mutex_queue);
        }

        if(pool->flags & f_shutdown){
            pthread_mutex_unlock(&pool->mutex_queue);
            pthread_exit(NULL);
        }

        task_queue_pop(&pool->task_queue, &task);
        pool->nb_th_working++;
        pthread_mutex_unlock(&pool->mutex_queue);
      
        if(!task.function){
            //unrecoverable error
            def_err_handler(err_val, "thpool_worker", err_val);
            pthread_exit(NULL);      
        }else{ 
            task.function(task.args);
            pool->nb_completed++;
        }
      
        if(pool->nb_completed <= pool->nb_submitted){
            pthread_cond_signal(&pool->cond_sub_equals_comp);
        }          
        pool->nb_th_working--;
    }
    return NULL;      
}
