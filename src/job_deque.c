#include "job_deque.h"
#include "common.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#define DEF_QUEUE_SIZE 64
#define QUEUE_REALLOC_COEFF 2

err_code task_queue_init(S_TASKQUEUE * queue){
    def_err_handler(!queue, "thqueue_create", err_null)

    queue->tasks = (S_TASK *)calloc(DEF_QUEUE_SIZE , sizeof(S_TASK));
    def_err_handler(!queue->tasks, "thqueue_create", err_alloc);

    queue->size = 0 ; 
    queue->max = DEF_QUEUE_SIZE;
    queue->start = 0;

    return err_ok ; 
}//tested works

static err_code task_queue_realloc(S_TASKQUEUE * queue){
    def_err_handler(!queue, "thqueue_realloc", err_null)

  

    queue->tasks = realloc(queue->tasks , queue->max * QUEUE_REALLOC_COEFF * sizeof(S_TASK));
    def_err_handler(!queue->tasks, "thqueue_realloc", err_alloc);


    if( queue->start + queue->size  > queue->max){
        uint32_t shift = queue->start + queue->size - queue->max;
        uint32_t new_start = queue->max * QUEUE_REALLOC_COEFF - shift;
       // printf("shift %d, start %d  newstart %d, size %d max %d, newmax %d\n", shift, queue->start, new_start, queue->size, queue->max,queue->max * QUEUE_REALLOC_COEFF);
        memmove( &queue->tasks[new_start], &queue->tasks[queue->start], shift * sizeof(S_TASK));
        queue->start += new_start;
    }
   
    queue->max *= QUEUE_REALLOC_COEFF;

    return err_ok;
}//tested ; WRONG 
//THIS FUNCTION DOES NOT WORK IM SO FUCKING DUMB


err_code add_front_deque(S_TASKQUEUE * dq, S_TASK * elem){
    /*
    dq -> not null & initialized
    adds elem to the front of the deque, see wiki of deque for more info
    */
    def_err_handler(!dq,"thqueue_deque dq", err_null);

    if(dq->max == dq->size){
        err_code failure = task_queue_realloc(dq);
        def_err_handler(failure, "add_front_deque", failure);
    }

    dq->start--; 
    if(dq->start < 0 ) dq->start += dq->max  ;

    dq->tasks[dq->start] = *elem; 
    dq->size++;

    return err_ok;
}//ok

void dump_deque(S_TASKQUEUE * dq);

err_code pop_back_deque(S_TASKQUEUE * dq, S_TASK * elem){
    /*
    */
    def_err_handler(!dq,"pop_back_deque", err_null);
    def_err_handler(!dq,"pop_back_deque", err_null);
    warning_handler(dq->size == 0, "pop_back_deque",err_val, elem = NULL ; return err_ok;);

    int64_t back_idx =  (dq->start + dq->size - 1 ) % dq->max;
    //printf("start %ld, size %ld, backidx %ld, dq.max %ld\n", dq->start, dq->size, back_idx, dq->max) ;
   if(! (dq->tasks[back_idx].function)){
       //printf("start %ld, size %ld, backidx %ld, dq.max %ld\n", dq->start, dq->size, back_idx, dq->max) ;
        //dump_deque(dq);
   }
    
    *elem = dq->tasks[back_idx];

    dq->size--;

    return err_ok;
}

void task_queue_destroy(S_TASKQUEUE * queue){
    if(queue){
        if(queue->tasks){
            free(queue->tasks);
        }
    }
}//tested ; works

bool task_queue_empty(S_TASKQUEUE * queue){
    return queue->size == 0 ;
}//tested ; inneficent ; should be a macro or smg


void dump_deque(S_TASKQUEUE * queue){
    for(uint32_t i = 0 ; i < queue->max ; i++){
        printf("i=%ld , q.task[i].fn=%p, q.task[i].args=%p\n",i, queue->tasks[i].function, queue->tasks[i].args);
    }
}
