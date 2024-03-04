#ifndef JOB_DEQUE_H
#define JOB_DEQUE_H

#include "common.h"

typedef struct s_task{
    void (* function)(void*); 
    void * args ;
}S_TASK;
/*
    simple wrapper for 
    function pointer with it's 
    args to pass to 
*/

typedef struct s_queue{
    uint32_t max ; 
    uint32_t size ; 

    uint32_t start ; 

    S_TASK * tasks ; 
}S_TASKQUEUE ; 
/*
    simple dequeue for tasks
    implemented with a dynamic array. 
    Maybe a linked list would have 
    been better but have strong 
    opinions about linked structures.
*/

extern err_code task_queue_init(S_TASKQUEUE * queue);
/*
    queue -> not null & not initialized ; 
    initializes the queue with a max size of 
    default queue 
*/

extern err_code task_queue_push_back(S_TASKQUEUE * queue , S_TASK * task);
#define task_queue_push task_queue_push_back
/*
    queue -> not null & initialized ; 
    task -> not null ; 
    pushes the task into the queue
    if the queue is full, reallocates it. 
*/

extern err_code task_queue_pop_front(S_TASKQUEUE * queue , S_TASK * task); 
#define task_queue_pop task_queue_pop_front
/*
    queue -> not null & initialized ; 
    task -> not null ; 
    pops the task from the queue
    if the queue is empty, returns a NULL pointer
*/

extern void task_queue_destroy(S_TASKQUEUE * queue);
/*
    queue -> not null ; 
    destroys the queue and frees the memory
    allocated to it. 
*/

extern bool task_queue_empty(S_TASKQUEUE * queue);
/*
    queue -> not null & initialized ; 
    returns true if the queue is empty
    false otherwise

    doesn't check for error ;
    tbf it could be a macro
*/

#ifdef debug 
extern void task_queue_print(S_TASKQUEUE * queue);
#endif

#endif 
