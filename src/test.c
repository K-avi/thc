#include "common.h"
#include "job_deque.h"
#include "thpool.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>


void  test_task(void * args){

    uint64_t i = (uint64_t) args;
   
    //sleep(1);
    //printf("task %d\n",  i);
    return;
}

int main(int argc, char** argv){

    
    /*
    S_TASKQUEUE queue; 
    

    task_queue_init(&queue);

    uint32_t nb_fn_call = 0 ; 
    uint32_t nb_push = 0 ; 
 
    uint32_t nb_miss = 0 ;
    for(int i = 0 ; i < 1000 ; i++){
        S_TASK task;
        task.function = test_task ;
        task.args = (void*) (1000 + i) ; 
        task_queue_push(&queue, &task);
        nb_push ++; 
        task_queue_push(&queue, &task);
        nb_push++; 

   

        task_queue_pop(&queue, &task);
        if(task.function){
        task.function(task.args);
        nb_fn_call++;
        }
        else nb_miss++;
    }
    
        printf("nbmiss=%d queue.size=%lu\n", nb_miss, queue.size);
    
    while(queue.size){
        S_TASK task ; 
        task_queue_pop(&queue, &task); 
        if(task.function){
            nb_fn_call++;
        task.function(task.args);
        }else{
            
            nb_miss++;
        }
    }
    printf("queue.size=%d\n", queue.size);
    printf("push=%d, fn_calls=%d, nb_miss=%d\n",nb_push, nb_fn_call, nb_miss);
    */
    

    S_THPOOL pool;
    S_TASK task;
    task.function = test_task;

    thpool_init(&pool);
    thpool_start(&pool);

    uint64_t i =0; 
    while( i < 1000){
       // printf("append task %d\n", i);
        task.args = (void*) i;
        thpool_append_task(&pool, &task);
    
        i++;
        
    }
    printf("i = %d \n", i);
   // sleep(5);
    thpool_wait_for_all(&pool);
    thpool_restart(&pool);

     i =0; 
    while( i < 1000){
       // printf("append task %d\n", i);
        task.args = (void*) i;
        thpool_append_task(&pool, &task);
    
        i++;
        
    }

    
    thpool_destroy(&pool );

    return 0;
}
