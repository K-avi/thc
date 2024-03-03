#ifndef THC_H 
#define THC_H 

#include "common.h"
#include "job_deque.h"
#include <pthread.h>
#include <stdint.h>


typedef struct thread_tab{
    uint32_t size ;
    pthread_t * threads ;
}S_THTAB;

typedef struct s_th_pool{

    S_TASKQUEUE task_queue ;
    S_THTAB thtab ;

    pthread_mutex_t mutex_queue;
    pthread_cond_t cond_queue;

    uint16_t flags ; 
    /*
    1<<0 -> pool is initialized
    1<<1 -> pool is shutting down nicely (stops accepting tasks)
    1<<2 -> pool is shutting down brutally (dies)
    */
}S_THPOOL ; 


extern err_code thpool_create(S_THPOOL * pool );
/*
    pool -> not null & not initialized ; 
    initializes the thread pool with a default queue size
    and a default number of threads ; 
    starts the thread
*/

extern err_code thpool_append_task(S_THPOOL * pool , S_TASK * task);
/*
    pool -> not null & initialized ; 
    task -> not null ; 
    appends the task to the thpool queue
*/

extern void thpool_destroy(S_THPOOL * pool );
/*
    pool -> not null ; 
    shutdowns the pool's thread,
    destroys the thread pool and frees it's memory
    Also frees the queue 
*/


#endif 
