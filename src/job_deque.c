#include "job_deque.h"
#include "common.h"

#define DEF_QUEUE_SIZE 32
#define get_back_queue(queue) ((queue->start + queue->size ) % queue->max)
#define QUEUE_REALLOC_COEFF 1.5 

err_code task_queue_init(S_TASKQUEUE * queue){
    def_err_handler(!queue, "thqueue_create", err_null)

    queue->tasks = (S_TASK *)calloc(DEF_QUEUE_SIZE , sizeof(S_TASK));
    def_err_handler(!queue->tasks, "thqueue_create", err_alloc);

    queue->size = 0 ; 
    queue->max = DEF_QUEUE_SIZE;
    queue->start = 0;

    return err_ok ; 
}//not tested

static err_code task_queue_realloc(S_TASKQUEUE * queue){
    def_err_handler(!queue, "thqueue_realloc", err_null)

    S_TASK * new_tasks = (S_TASK *)realloc(queue->tasks, queue->max * QUEUE_REALLOC_COEFF * sizeof(S_TASK));
    def_err_handler(!new_tasks, "thqueue_realloc", err_alloc);

    queue->tasks = new_tasks;
    queue->max *= QUEUE_REALLOC_COEFF;

    return err_ok;
}//not tested

err_code task_queue_push_back(S_TASKQUEUE * queue , S_TASK * task){
    def_err_handler(!queue, "thqueue_push", err_null)
    def_err_handler(!task, "thqueue_push", err_null)

    if(queue->size == queue->max){
        err_code failure = task_queue_realloc(queue);
        def_err_handler(failure, "thqueue_push", failure);
    }

    queue->tasks[get_back_queue(queue)] = *task;
    queue->size++;
   
    return err_ok;
}//not tested

err_code task_queue_pop_front(S_TASKQUEUE * queue , S_TASK * task){
    def_err_handler(!queue, "thqueue_pop", err_null)
    def_err_handler(!task, "thqueue_pop", err_null)

    if(queue->size == 0){
        task = NULL ; 
        def_war_handler(err_null, "thqueue_pop", err_null);
        return err_ok ;
    }

    *task = queue->tasks[queue->start];
    queue->size--;
    queue->start = (queue->start + 1) % queue->max;

    return err_ok;
}//not tested

void task_queue_destroy(S_TASKQUEUE * queue){
    if(queue){
        if(queue->tasks){
            free(queue->tasks);
        }
    }
}//not tested

bool task_queue_empty(S_TASKQUEUE * queue){
    return queue->size == 0 ;
}//not tested
